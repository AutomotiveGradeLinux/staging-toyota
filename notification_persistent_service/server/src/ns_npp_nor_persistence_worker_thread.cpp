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
/// \brief    This file contain declaration of class CNorPersistenceWorker and CArchive.
///       Class CNorPersistenceWorker holds the implementation for worker thread.
///       Class CArchive holds the implementation for archive.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_timer_if.hpp>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_utility_sys.hpp>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_np_service_nor_persistence.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <boost/bind.hpp>
#include <map>
#include <utility>
#include <string>

#include <native_service/ns_np_service_nor_persistence_internal.h>
#include "ns_npp_types.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_fs_directory.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_nor_persistence_worker_thread.h"



typedef std::string NotificationName;
typedef std::map<NotificationName, CTimerHelper *> TTimerList;
static TTimerList g_mtimers;  /// List of all the timers for immediate persistence notifications
CMutex g_objmutextimerdata;  /// Mutex object on g_mtimers

// Template function for NorWorkerCallback
template <typename C, eFrameworkunifiedStatus(C::*M)(HANDLE)> EFrameworkunifiedStatus NorWorkerCallback(HANDLE hthread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  C *l_pClass = static_cast<C *>(FrameworkunifiedGetThreadSpecificData(hthread));

  if (l_pClass) {  // LCOV_EXCL_BR_LINE 6: l_pClass can't be NULL
    l_estatus = (l_pClass->*M)(hthread);
  }
  return l_estatus;
}

static FrameworkunifiedProtocolCallbackHandler aServiceHandlers[] = {  // NOLINT (readability/naming)
  { NOR_PERSISTENCE_TIMER_START,      NorWorkerCallback<CNorPersistenceWorker,
    &CNorPersistenceWorker::OnNorPersistenceTimerStart>},
  { NOR_PERSISTENCE_REGISTER,       NorWorkerCallback<CNorPersistenceWorker,
    &CNorPersistenceWorker::RegisterImmediatePersistNotification>},
  { NOR_PERSISTENCE_UNREGISTER,     NorWorkerCallback<CNorPersistenceWorker,
    &CNorPersistenceWorker::UnregisterImmediatePersistNotification>},
  { NOR_PERSISTENCE_ONSHUTDOWN,       NorWorkerCallback<CNorPersistenceWorker,
    &CNorPersistenceWorker::OnShutdown>},
  { NOR_PERSISTENCE_CHANGE_CATEGORY,  NorWorkerCallback<CNorPersistenceWorker,
    &CNorPersistenceWorker::OnCategoryChange>}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPNorPersistenceWorkerOnStart
/// Callback method on start of worker thread
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPNorPersistenceWorkerOnStart(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (NULL != hthread) {  // LCOV_EXCL_BR_LINE 6: hthread can't be NULL
    // Note: this variable is made static just to ignore the resource leak showing in coverity analysis
    static CNorPersistenceWorker *l_pcnorpersistenceworker = new(std::nothrow) CNorPersistenceWorker;

    if (NULL != l_pcnorpersistenceworker) {  // LCOV_EXCL_BR_LINE 5: new's error case
      l_estatus = FrameworkunifiedSetThreadSpecificData(hthread, l_pcnorpersistenceworker);

      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        l_estatus = FrameworkunifiedAttachCallbacksToDispatcher(
                      hthread, AppName, aServiceHandlers, static_cast<UI_32>(_countof(aServiceHandlers)));
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        delete l_pcnorpersistenceworker;
        l_pcnorpersistenceworker = NULL;
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 5: new's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Worker Object Null.");
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 6: hthread can't be NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// NSPNorPersistenceWorkerOnStop
/// Callback method on stop of worker thread
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus NSPNorPersistenceWorkerOnStop(HANDLE hthread) {  // LCOV_EXCL_START 200: can't test
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  if (NULL != hthread) {
    CNorPersistenceWorker *l_pcnorpersistenceworker = static_cast<CNorPersistenceWorker *>(FrameworkunifiedGetThreadSpecificData(
                                                                                             hthread));

    if (NULL != l_pcnorpersistenceworker) {
      delete l_pcnorpersistenceworker;
      l_pcnorpersistenceworker = NULL;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Worker Object Null");
      l_estatus = eFrameworkunifiedStatusNullPointer;
    }

    if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSetThreadSpecificData(hthread, NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSetThreadSpecificData error, status=%d", l_estatus);
    }

    PCSTR l_cservice = FrameworkunifiedGetAppName(hthread);
    if (NULL != l_cservice) {
      if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedDetachServiceFromDispatcher(hthread, l_cservice))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error detaching service %s from dispatcher, status=%d", l_cservice, l_estatus);
      }
    } else {
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetAppName returned NULL");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Thread App Handle Null.");
    l_estatus = eFrameworkunifiedStatusInvldHandle;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");

  return l_estatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class : CNorPersistenceWorker
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CNorPersistenceWorker
/// Constructor of CNorPersistenceWorker class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNorPersistenceWorker::CNorPersistenceWorker() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CNorPersistenceWorker
/// Destructor of CNorPersistenceWorker class
////////////////////////////////////////////////////////////////////////////////////////////////////
CNorPersistenceWorker::~CNorPersistenceWorker() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnNorPersistenceTimerStart
/// This function starts the respective timer of the notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNorPersistenceWorker::OnNorPersistenceTimerStart(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusFail;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  TNorPersistenceNotifInfoHeader *l_ptNorPersistentData = NULL;
  UI_32 l_ui32ReceivedDataLength = FrameworkunifiedGetMsgLength(f_hThread);
  UI_8 *l_pui32ReceivedData = new(std::nothrow) UI_8[l_ui32ReceivedDataLength];

  if (l_ui32ReceivedDataLength >= sizeof(TNorPersistenceNotifInfoHeader)) {  // LCOV_EXCL_BR_LINE 200: l_ui32ReceivedDataLength is bigger than sizeof(TNorPersistenceNotifInfoHeader)  // NOLINT[whitespace/line_length]
    if (NULL != l_pui32ReceivedData) {  // LCOV_EXCL_BR_LINE 5: new's error case
      std::memset(l_pui32ReceivedData, 0, l_ui32ReceivedDataLength);
      l_estatus = FrameworkunifiedGetMsgDataOfSize(f_hThread, l_pui32ReceivedData, l_ui32ReceivedDataLength, eSMRRelease);
      if (eFrameworkunifiedStatusOK == l_estatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
        TTimerList::iterator l_itTimerList;
        l_ptNorPersistentData = static_cast<TNorPersistenceNotifInfoHeader *>(static_cast<PVOID>(l_pui32ReceivedData));

        g_objmutextimerdata.WriteLock();
        // find the corresponding timer object in the list of timers
        l_itTimerList = g_mtimers.find(l_ptNorPersistentData->m_cnotificationname);
        if (g_mtimers.end() != l_itTimerList) {  // LCOV_EXCL_BR_LINE 200: the notification must be registered
          if (NULL != (l_itTimerList->second)) {  // LCOV_EXCL_BR_LINE 200: l_itTimerList->second can't be null
            if (NULL != (l_itTimerList->second)->m_pui8HeaderAndData) {
              delete[](l_itTimerList->second)->m_pui8HeaderAndData;
              (l_itTimerList->second)->m_pui8HeaderAndData = NULL;
            }

            // Update specific immediate persistence notification data
            (l_itTimerList->second)->m_pui8HeaderAndData = l_pui32ReceivedData;

            // if corresponding timer is not already running start it.
            if (!(l_itTimerList->second)->IsRunning()) {
              (l_itTimerList->second)->Start();
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Timer started, Notfn=%s", l_ptNorPersistentData->m_cnotificationname);
            } else {
              FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Timer already running for notfn %s. Data updated",
                     l_ptNorPersistentData->m_cnotificationname);
            }

            l_estatus = eFrameworkunifiedStatusOK;
          } else {
            // LCOV_EXCL_START 200: l_itTimerList->second can't be null
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_estatus = eFrameworkunifiedStatusNullPointer;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Immediate persistence: Timer object NULL for Notfn:%s.",
                   l_ptNorPersistentData->m_cnotificationname);
            // LCOV_EXCL_STOP
          }
        } else {
          // LCOV_EXCL_START 200: the notification must be registered
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          l_estatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Immediate persistence notification not registered. Notification: %s ",
                 l_ptNorPersistentData->m_cnotificationname != 0 ? l_ptNorPersistentData->m_cnotificationname : NULL);
          // LCOV_EXCL_STOP
        }
        g_objmutextimerdata.Unlock();
      } else {
        // LCOV_EXCL_START 4: NSFW error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "FrameworkunifiedGetMsgDataOfSize failed while retrieving immediate persistence notfn data. Status:0x%x ", l_estatus);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 5: new's error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to allocate data. Can't persist.");
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 200: l_ui32ReceivedDataLength is bigger than sizeof(TNorPersistenceNotifInfoHeader)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_estatus = eFrameworkunifiedStatusInvldBuf;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid data length received. Can't persist.");
    // LCOV_EXCL_STOP
  }

  if ((eFrameworkunifiedStatusOK != l_estatus) && (NULL != l_pui32ReceivedData)) {
    delete[] l_pui32ReceivedData;
    l_pui32ReceivedData = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RegisterImmediatePersistNotification
/// It creates a timer corresponding to the notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNorPersistenceWorker::RegisterImmediatePersistNotification(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  TImmediatePersistenceRegisterNotifInfo l_tRegisterNotifInfo = {};

  if (sizeof(l_tRegisterNotifInfo) == FrameworkunifiedGetMsgLength(hthread)) {  // LCOV_EXCL_BR_LINE 200: the size of msg must be sizeof(TImmediatePersistenceUnregisterNotifInfo)  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(hthread, &l_tRegisterNotifInfo,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                       sizeof(TImmediatePersistenceRegisterNotifInfo), eSMRRelease))) {
      g_objmutextimerdata.WriteLock();

      TTimerList::iterator l_itTimerList = g_mtimers.find(l_tRegisterNotifInfo.m_cnotificationname);
      if (l_itTimerList != g_mtimers.end()) {  // LCOV_EXCL_BR_LINE 6: notification can't be registered again
        // LCOV_EXCL_START 6: notification can't be registered again
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification %s is already registered for immediate data persistence ",
               l_tRegisterNotifInfo.m_cnotificationname);
        // LCOV_EXCL_STOP
      } else {
        // create timer corresponding to notification
        CTimerHelper *l_pTimerHelper = new CTimerHelper();
        if (NULL != l_pTimerHelper) {  // LCOV_EXCL_BR_LINE 5: new's error case
          l_pTimerHelper->SetTime(l_tRegisterNotifInfo.m_uidelay, 0);
          // todo: modify SetNotifyMethod of timer NSTimer class to accept sender handle as well.
          //       This will help to have only one handle, instead of one handle per timer.
          l_pTimerHelper->SetNotifyMethod(static_cast<UI_16>(l_pTimerHelper->GetCmdId()), FrameworkunifiedGetAppName(hthread));

          // associate timer's timeout with the callback function.
          FrameworkunifiedAttachCallbackToDispatcher(hthread, TIMER_QUE, l_pTimerHelper->GetCmdId(),
                                        boost::bind(&CTimerHelper::OnTimeOut, l_pTimerHelper, hthread));
          // add created timer in timers list
          g_mtimers.insert(std::make_pair(l_tRegisterNotifInfo.m_cnotificationname, l_pTimerHelper));
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notification %s is registered for immediate data persistence ",
                 l_tRegisterNotifInfo.m_cnotificationname != 0 ? l_tRegisterNotifInfo.m_cnotificationname : NULL);
        }
      }

      g_objmutextimerdata.Unlock();
    } else {
      // LCOV_EXCL_START 4: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error getting message data for registering Immediate persistent notification, status: 0x%x", l_estatus);
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 200: the size of msg must be sizeof(TImmediatePersistenceUnregisterNotifInfo)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "FrameworkunifiedGetMsgLength and size of TImmediatePersistenceRegisterNotifInfo mismatched."
           "Length received %d, buffer available: %ld ",
           FrameworkunifiedGetMsgLength(hthread), static_cast<long int>(sizeof(TImmediatePersistenceRegisterNotifInfo))); // NOLINT (runtime/int)
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UnregisterImmediatePersistNotification
/// It deletes a timer corresponding to the notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNorPersistenceWorker::UnregisterImmediatePersistNotification(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  TImmediatePersistenceUnregisterNotifInfo l_tRegisterNotifInfo = {};

  if (sizeof(l_tRegisterNotifInfo) == FrameworkunifiedGetMsgLength(hthread)) {  // LCOV_EXCL_BR_LINE 200: the size of msg must be sizeof(TImmediatePersistenceUnregisterNotifInfo)  // NOLINT[whitespace/line_length]
    if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(hthread, &l_tRegisterNotifInfo,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                    sizeof(TImmediatePersistenceUnregisterNotifInfo), eSMRRelease))) {
      g_objmutextimerdata.WriteLock();

      TTimerList::iterator l_itTimerList = g_mtimers.find(l_tRegisterNotifInfo.m_cnotificationname);
      if (l_itTimerList != g_mtimers.end()) {  // LCOV_EXCL_BR_LINE 6: notification can't be unregistered again
        if (NULL != (l_itTimerList->second)) {  // LCOV_EXCL_BR_LINE 200: l_itTimerList->second can't be null
          EFrameworkunifiedStatus eStatus;
          // detach callbacks
          if (eFrameworkunifiedStatusOK != (eStatus = FrameworkunifiedDetachCallbackFromDispatcher(hthread, TIMER_QUE,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                                         (l_itTimerList->second)->GetCmdId()))) {
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedDetachCallbackFromDispatcher Failed Status:0x%x ", eStatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          }
          delete l_itTimerList->second;
          l_itTimerList->second = NULL;
        }
        g_mtimers.erase(l_itTimerList);
      } else {
        // LCOV_EXCL_START 6: notification can't be registered again
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
               "Can't unregister. Notification %s is not a registered notification for immediate data persistence.",
               l_tRegisterNotifInfo.m_cnotificationname != 0 ? l_tRegisterNotifInfo.m_cnotificationname : NULL);
        // LCOV_EXCL_STOP
      }

      g_objmutextimerdata.Unlock();
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             "Notification %s is unregistered for immediate data persistence, FrameworkunifiedGetMsgDataOfSize Error, status: 0x%x",
             l_tRegisterNotifInfo.m_cnotificationname != 0 ? l_tRegisterNotifInfo.m_cnotificationname : NULL,
             l_estatus);
    }
  } else {
    // LCOV_EXCL_START 200: the size of msg must be sizeof(TImmediatePersistenceUnregisterNotifInfo)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "FrameworkunifiedGetMsgLength and size of TImmediatePersistenceUnregisterNotifInfo mismatched."
           "Length received %d, buffer available: %ld ",
           FrameworkunifiedGetMsgLength(hthread), static_cast<long int>(sizeof(TImmediatePersistenceUnregisterNotifInfo))); // NOLINT (runtime/int)
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnShutdown
/// This API saves or deletes the data of NOR irrespective of delay depending on the message data
/// received from parent thread during shutdown.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNorPersistenceWorker::OnShutdown(HANDLE f_hThread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  UI_32 l_uiPersistCategoryFlag = 0;

  TImmediateShutdown l_tImmShutdown = {};

  CTimerHelper *l_pTimerHelper = NULL;

  // iterator of list of nor notifications and its timers
  TTimerList::iterator l_itTimerList;

  TNorPersistenceNotifInfoHeader *l_ptImmediateNotfnHeader = NULL;

  if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(f_hThread, &l_tImmShutdown, sizeof(l_tImmShutdown),  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                       eSMRRelease))) {
    l_uiPersistCategoryFlag = l_tImmShutdown.f_uinotificationpersistentservicepersistcategoryflag;

    // if quick shutdown is received then just stop the timer and dont save the data
    if (eFrameworkunifiedQuickShutdown == l_tImmShutdown.f_eshutdowntype) {
      for (l_itTimerList = g_mtimers.begin(); l_itTimerList != g_mtimers.end(); l_itTimerList++) {
        l_pTimerHelper = l_itTimerList->second;

        if (NULL != l_pTimerHelper && l_pTimerHelper->IsRunning()) {
          // stop the timer
          l_pTimerHelper->Stop();
        }
      }
    } else {  // stop the timer and save the data
      for (l_itTimerList = g_mtimers.begin(); l_itTimerList != g_mtimers.end(); l_itTimerList++) {
        l_pTimerHelper = l_itTimerList->second;

        if (NULL != l_pTimerHelper && l_pTimerHelper->IsRunning()) {
          // stop the timer
          l_pTimerHelper->Stop();

          l_ptImmediateNotfnHeader = static_cast<TNorPersistenceNotifInfoHeader *>
                  (static_cast<PVOID>(l_pTimerHelper->m_pui8HeaderAndData));

          if (NULL != l_ptImmediateNotfnHeader) {  // LCOV_EXCL_BR_LINE 200: l_ptImmediateNotfnHeader can't be null
            if (eFrameworkunifiedUserData       != (l_ptImmediateNotfnHeader->m_epersistcategory & l_uiPersistCategoryFlag) &&
                eFrameworkunifiedFactoryData      != (l_ptImmediateNotfnHeader->m_epersistcategory & l_uiPersistCategoryFlag) &&
                eFrameworkunifiedFactoryCustomerData  != (l_ptImmediateNotfnHeader->m_epersistcategory & l_uiPersistCategoryFlag) &&
                eFrameworkunifiedDealerData     != (l_ptImmediateNotfnHeader->m_epersistcategory & l_uiPersistCategoryFlag)) {
              // save the new data to persistent memory
              l_pTimerHelper->OnTimeOut(f_hThread);
            }
          } else {
            // LCOV_EXCL_START 200: l_ptImmediateNotfnHeader can't be null
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Immediate notification data header is NULL");
            l_estatus = eFrameworkunifiedStatusFail;
            // LCOV_EXCL_STOP
          }
        }
      }
    }
  } else {
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize failed for protocol NOR_PERSISTENCE_ONSHUTDOWN, status : %d",
           l_estatus);
    // LCOV_EXCL_STOP
  }

  if (eFrameworkunifiedStatusOK != (l_estatus = FrameworkunifiedSendParent(f_hThread, NOR_PERSISTENCE_ONSHUTDOWN_ACK,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                 sizeof(l_uiPersistCategoryFlag), &l_uiPersistCategoryFlag))) {
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendParent failed for protocol NOR_PERSISTENCE_ONSHUTDOWN_ACK, status : %d",
           l_estatus);
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnCategoryChange
/// This callback is called when category of immediate persistence is changed
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CNorPersistenceWorker::OnCategoryChange(HANDLE hthread) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != hthread) {  // LCOV_EXCL_BR_LINE 4: NSFW error case
    UI_32 l_ui32ReceivedDataLength = FrameworkunifiedGetMsgLength(hthread);

    if (l_ui32ReceivedDataLength >= sizeof(TImmediatePersistenceChangeCategory)) {  // LCOV_EXCL_BR_LINE 200: l_ui32ReceivedDataLength is bigger than sizeof(TImmediatePersistenceChangeCategory)  // NOLINT[whitespace/line_length]
      UI_8 *l_pui32ReceivedData = new(std::nothrow) UI_8[l_ui32ReceivedDataLength];

      if (NULL != l_pui32ReceivedData) {  // LCOV_EXCL_BR_LINE 5: new's error case
        std::memset(l_pui32ReceivedData, 0, l_ui32ReceivedDataLength);
        if (eFrameworkunifiedStatusOK == (l_estatus = FrameworkunifiedGetMsgDataOfSize(hthread, l_pui32ReceivedData, l_ui32ReceivedDataLength,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                             eSMRRelease))) {
          TImmediatePersistenceChangeCategory *l_pChangePersistentCategory =
                  static_cast<TImmediatePersistenceChangeCategory *>(static_cast<PVOID>(l_pui32ReceivedData));
          PUI_8 l_pui8Data = l_pui32ReceivedData + sizeof(TImmediatePersistenceChangeCategory);

          TTimerList::iterator l_itTimerList;

          g_objmutextimerdata.WriteLock();
          // find the corresponding timer object in the list of timers
          l_itTimerList = g_mtimers.find(l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname);
          if (g_mtimers.end() != l_itTimerList) {  // LCOV_EXCL_BR_LINE 6: notification must be in list
            CTimerHelper *l_pTimerHelper = l_itTimerList->second;

            if (NULL != l_pTimerHelper && l_pTimerHelper->IsRunning()) {
              TNorPersistenceNotifInfoHeader *l_ptImmediateNotfnHeader =
                static_cast<TNorPersistenceNotifInfoHeader *>(static_cast<PVOID>(l_pTimerHelper->m_pui8HeaderAndData));

              if (NULL != l_ptImmediateNotfnHeader) {  // LCOV_EXCL_BR_LINE 200: l_ptImmediateNotfnHeader can't be null
                FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Changing category from %d to %d for notfn %s in dataHdr",
                       l_ptImmediateNotfnHeader->m_epersistcategory,
                       l_pChangePersistentCategory->m_tnornotifInfoheader.m_epersistcategory,
                       l_ptImmediateNotfnHeader->m_cnotificationname);

                l_ptImmediateNotfnHeader->m_epersistcategory =
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_epersistcategory;
              }
            }

            EFrameworkunifiedPersistCategory l_eOldPersistCategory = l_pChangePersistentCategory->m_eoldpersistcategory;

            FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__, "Changing persist category of %s from %d to %d",
                   l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname,
                   l_eOldPersistCategory,
                   l_pChangePersistentCategory->m_tnornotifInfoheader.m_epersistcategory);

            // write data to emmc
            l_estatus = SynchronousWritePersistentData(
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_cpublishername,
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname,
                        static_cast<PVOID>(l_pui8Data),
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_uimsgsize,
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_uidelay,
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_uimaxmsglength,
                        l_pChangePersistentCategory->m_tnornotifInfoheader.m_epersistcategory);

            // Create the old file path string to be removed
            std::string l_cPath = IMMEDIATE_PERSISTENCE_STORAGE_V2;
            std::string l_cTempPath = l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname;
            size_t l_uiTempPathSize = 0;

            // replace all '/' by '_'
            l_uiTempPathSize = l_cTempPath.find_first_of("/");
            while (std::string::npos != l_uiTempPathSize) {
              l_cTempPath[l_uiTempPathSize] = '_';
              l_uiTempPathSize = l_cTempPath.find_first_of("/", l_uiTempPathSize + 1);
            }

            switch (l_eOldPersistCategory) {
              case eFrameworkunifiedFactoryData:
                l_cPath.append(FACTORYDATADIR);
                break;
              case eFrameworkunifiedFactoryCustomerData:
                l_cPath.append(FACTORYCUSTOMERDATADIR);
                break;
              case eFrameworkunifiedDealerData:
                l_cPath.append(DEALERDATADIR);
                break;
              case eFrameworkunifiedUserData:
              default: {
                  l_cPath.append(USERDATADIR);
                  l_cPath.append(ALLUSERAPPDATADIR);
                }
                break;
            }

            l_cPath.append(IMMEDIATEDATADIR);
            l_cPath.append(l_cTempPath);

            l_cPath.append(l_pChangePersistentCategory->m_tnornotifInfoheader.m_cpublishername);

            if (0 != unlink(l_cPath.c_str())) {
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error deleting file %s, errno=%d", l_cPath.c_str(), errno);
            }
          } else {
            // LCOV_EXCL_START 6: notification must be in list
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            l_estatus = eFrameworkunifiedStatusFail;
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Can't update persistent category, notfn %s not found in g_mtimers map",
                   l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname != 0 ?
                   l_pChangePersistentCategory->m_tnornotifInfoheader.m_cnotificationname : NULL);
            // LCOV_EXCL_STOP
          }
          g_objmutextimerdata.Unlock();
        } else {
          // LCOV_EXCL_START 4: NSFW error case
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedGetMsgDataOfSize failed while retrieving"
                                         "immediate persistence notfn data. Status:0x%x ", l_estatus);
          // LCOV_EXCL_STOP
        }

        delete[] l_pui32ReceivedData;
        l_pui32ReceivedData = NULL;
      } else {
        // LCOV_EXCL_START 5: new's error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusNullPointer;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Unable to allocate data. Can't persist, errno=%d", errno);
        // LCOV_EXCL_STOP
      }
    } else {
      // LCOV_EXCL_START 200: l_ui32ReceivedDataLength is bigger than sizeof(TImmediatePersistenceChangeCategory)
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusInvldBuf;
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid data length received %d. Can't persist.", l_ui32ReceivedDataLength);
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 4: NSFW error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Invalid param, hthread is NULL");
    l_estatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

/////////////////////////////
/// Class: CTimerHelper
/////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CTimerHelper
/// Constructor of CTimerHelper class
////////////////////////////////////////////////////////////////////////////////////////////////////
CTimerHelper::CTimerHelper() {
  static UI_32 l_uiTimerCounter  = PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD;
  l_uiTimerCounter++;
  m_uiCmdId = l_uiTimerCounter;
  m_pui8HeaderAndData = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CTimerHelper
/// Destructor of CTimerHelper class
////////////////////////////////////////////////////////////////////////////////////////////////////
CTimerHelper::~CTimerHelper() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != m_pui8HeaderAndData) {
    delete[] m_pui8HeaderAndData;
    m_pui8HeaderAndData = NULL;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetCmdId
/// Returns id for timer
////////////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CTimerHelper::GetCmdId() {
  return m_uiCmdId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OnTimeOut
/// This function writes the corresponding data in NOR when timer expires.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CTimerHelper::OnTimeOut(HANDLE hthread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;

  g_objmutextimerdata.ReadLock();
  if (NULL != m_pui8HeaderAndData) {  // LCOV_EXCL_BR_LINE 200: m_pui8HeaderAndData can't be null
    TNorPersistenceNotifInfoHeader *l_ptImmediateNotfnHeader =
      static_cast<TNorPersistenceNotifInfoHeader *>(static_cast<PVOID>(m_pui8HeaderAndData));
    PUI_8 l_pui8Data = m_pui8HeaderAndData + sizeof(TNorPersistenceNotifInfoHeader);
    HANDLE l_hReceiverMq = McOpenSender(l_ptImmediateNotfnHeader->m_cpublishername);

    // write data to emmc
    l_estatus = SynchronousWritePersistentData(l_ptImmediateNotfnHeader->m_cpublishername,
                                               l_ptImmediateNotfnHeader->m_cnotificationname,
                                               static_cast<PVOID>(l_pui8Data),
                                               l_ptImmediateNotfnHeader->m_uimsgsize,
                                               l_ptImmediateNotfnHeader->m_uidelay,
                                               l_ptImmediateNotfnHeader->m_uimaxmsglength,
                                               l_ptImmediateNotfnHeader->m_epersistcategory);

    // update corresponding application about data written status.
    if (NULL != l_hReceiverMq) {  // LCOV_EXCL_BR_LINE 5: NSFW error case
      NC_ImmediateWriteAck l_tWriteAck = {};
      std::strncpy(l_tWriteAck.notificationName, l_ptImmediateNotfnHeader->m_cnotificationname,
                   sizeof(l_tWriteAck.notificationName) - 1);
      l_tWriteAck.eStatus = l_estatus;

      if (eFrameworkunifiedStatusOK != (l_estatus = McSend(l_hReceiverMq, AppName, NPS_IMMEDIATE_WRITE_ACK, sizeof(l_tWriteAck),  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
                                              &l_tWriteAck))) {
        // LCOV_EXCL_START 4: NSFW error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Error Sending NPS_IMMEDIATE_WRITE_ACK to %s for %s, Status: %d",
               l_ptImmediateNotfnHeader->m_cpublishername != 0 ? l_ptImmediateNotfnHeader->m_cpublishername : NULL,
               l_ptImmediateNotfnHeader->m_cnotificationname != 0 ?
               l_ptImmediateNotfnHeader->m_cnotificationname : NULL,
               l_estatus);
        // LCOV_EXCL_STOP
      }
      McClose(l_hReceiverMq);
      l_hReceiverMq = NULL;
    } else {
      // LCOV_EXCL_START 5: NSFW error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "Error creating sender handle for service %s, Notification name %s Write Status: %d",
             l_ptImmediateNotfnHeader->m_cpublishername != 0 ? l_ptImmediateNotfnHeader->m_cpublishername : NULL,
             l_ptImmediateNotfnHeader->m_cnotificationname != 0 ?
             l_ptImmediateNotfnHeader->m_cnotificationname : NULL,
             l_estatus);
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }
  } else {
    // LCOV_EXCL_START 200: m_pui8HeaderAndData can't be null
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Not writing immediate persistence data. Data is NULL");
    l_estatus = eFrameworkunifiedStatusNullPointer;
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // LCOV_EXCL_STOP
  }
  g_objmutextimerdata.Unlock();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

/// EOF
