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
/// \ingroup  tag_NativeService
/// \brief    This file contains the implementation for the synchronization
///       API's class.
///
///////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_framework_sync.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_logger_if.h>
#include <string>
#include <algorithm>
#include <utility>
#include "frameworkunified_framework_core.h"

/// initialization of static members of class
CFrameworkunifiedSyncData *CFrameworkunifiedSyncData::m_psSyncData = NULL;

pthread_spinlock_t CFrameworkunifiedSyncData::m_pSyncLock;
static pthread_mutex_t g_instance_lock = PTHREAD_MUTEX_INITIALIZER;

// Template function to invoke callback function of CFrameworkunifiedSyncData class
template <typename C, eFrameworkunifiedStatus(C::*M)(HANDLE)> EFrameworkunifiedStatus SyncDataCallback(HANDLE hThread) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  C *l_pClass = C::FrameworkunifiedGetSyncDataInstance();

  if (NULL != l_pClass) {
    l_eStatus = (l_pClass->*M)(hThread);
  }

  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedSyncData
/// Constructor of CFrameworkunifiedSyncData class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedSyncData::CFrameworkunifiedSyncData(): m_bSyncThreadStarted(FALSE),
  m_mSyncDataMap(NULL),
  m_hSyncThreadAppHandle(NULL),
  m_hAppHandle(NULL),
  m_hSyncThreadMsgQHandle(NULL) {
  pthread_spin_init(&m_pSyncLock, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CFrameworkunifiedSyncData
/// Destructor of CFrameworkunifiedSyncData class
////////////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedSyncData::~CFrameworkunifiedSyncData() {
  pthread_spin_destroy(&m_pSyncLock);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedStartNotificationSync
/// This method is used to start the synchronization notification thread.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedStartNotificationSync(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  pthread_spin_lock(&m_pSyncLock);
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    m_hAppHandle = hApp;
    std::string  l_cThreadName =  FrameworkunifiedGetAppName(hApp);
    // l_cThreadName.append("SyncThread");
    std::reverse(l_cThreadName.begin(), l_cThreadName.end());

    if (NULL == m_hSyncThreadMsgQHandle) {
      // Create Sync Thread
      m_hSyncThreadMsgQHandle = FrameworkunifiedCreateChildThread(hApp,
                                                     l_cThreadName.c_str(),
                                                     SyncDataCallback<CFrameworkunifiedSyncData,
                                                     &CFrameworkunifiedSyncData::FrameworkunifiedSyncDataThreadStart>,
                                                     SyncDataCallback<CFrameworkunifiedSyncData,
                                                     &CFrameworkunifiedSyncData::FrameworkunifiedSyncDataThreadStop>);
      if (NULL != m_hSyncThreadMsgQHandle) {
        if (FALSE == m_bSyncThreadStarted) {
          if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStartChildThread(hApp,
                                                               m_hSyncThreadMsgQHandle,
                                                               0,
                                                               NULL))) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Failed to start Thread.");
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, " Sync Data Thread Started");
            m_bSyncThreadStarted = TRUE;
          }
        } else {
          l_eStatus = eFrameworkunifiedStatusThreadAlreadyRunning;
        }

      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Sync Data Thread Message Queue Null");
        l_eStatus = eFrameworkunifiedStatusNullPointer;
      }
    } else {
      l_eStatus =  eFrameworkunifiedStatusThreadAlreadyRunning;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  pthread_spin_unlock(&m_pSyncLock);

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedStartNotificationSync
/// This method is used to stop the synchronization notification thread.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedStopNotificationSync(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (NULL != m_hSyncThreadMsgQHandle) {
      pthread_spin_lock(&m_pSyncLock);
      if (eFrameworkunifiedStatusOK != (l_eStatus = (FrameworkunifiedStopChildThread(hApp,
                                                           m_hSyncThreadMsgQHandle,
                                                           0,
                                                           NULL)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedStopChildThread Sync Thread Failed.");
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "FrameworkunifiedStopChildThread Sync Thread Success.");
        m_bSyncThreadStarted = FALSE;
      }
      pthread_spin_unlock(&m_pSyncLock);
    } else {
      l_eStatus = eFrameworkunifiedStatusThreadNotExist;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetSyncDataInstance
/// This function is used to get the singleton instance of class.
////////////////////////////////////////////////////////////////////////////////////////////////////
CFrameworkunifiedSyncData *CFrameworkunifiedSyncData::FrameworkunifiedGetSyncDataInstance() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");

  if (NULL == m_psSyncData) {
    pthread_mutex_lock(&g_instance_lock);
    if (NULL == m_psSyncData) {
      CFrameworkunifiedSyncData *l_pFrameworkunifiedSyncData = new(std::nothrow)CFrameworkunifiedSyncData();
      m_psSyncData = l_pFrameworkunifiedSyncData;
    }
    pthread_mutex_unlock(&g_instance_lock);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return m_psSyncData;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedReleaseSyncDataInstance
/// This function is used to release the instance of class.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedReleaseSyncDataInstance() {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_psSyncData) {
    pthread_mutex_lock(&g_instance_lock);
    if (NULL != m_psSyncData) {
      delete m_psSyncData;
      m_psSyncData = NULL;
    }
    pthread_mutex_unlock(&g_instance_lock);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSyncDataThreadStart
/// This function is callback function on the start of the sync thread.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedSyncDataThreadStart(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != hThread) {
    // Set Sync Thread Handle
    m_hSyncThreadAppHandle = hThread;

    if (NULL == m_mSyncDataMap) {
      m_mSyncDataMap = new(std::nothrow)TSyncDataPacketList();
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSyncDataThreadStop
/// This function is callback function on the stop of the sync thread.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedSyncDataThreadStop(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");

  if (NULL != m_mSyncDataMap) {
    for (TSyncDataPacketItr l_itSyncDataPacketItr = m_mSyncDataMap->begin() ;
         l_itSyncDataPacketItr != m_mSyncDataMap->end();) {
      SFrameworkunifiedSyncDataPacket *l_ptDataPacket = l_itSyncDataPacketItr->second;

      if (NULL != l_ptDataPacket) {
        if (NULL != l_ptDataPacket->m_pNotificationData) {
          delete[](static_cast<PCHAR>(l_ptDataPacket->m_pNotificationData));
          l_ptDataPacket->m_pNotificationData = NULL;
        }
        delete l_ptDataPacket;
        l_ptDataPacket = NULL;
        m_mSyncDataMap->erase(l_itSyncDataPacketItr++);
      } else {
        l_itSyncDataPacketItr++;
      }
    }

    delete m_mSyncDataMap;
    m_mSyncDataMap = NULL;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusExit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSubscribeNotificationWithDataSync
/// This function is used to subscribe to notifications.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedSubscribeNotificationWithDataSync(const std::string &f_cNotification) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  pthread_spin_lock(&m_pSyncLock);
  if (TRUE != f_cNotification.empty() && NULL != m_hSyncThreadAppHandle) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(
                                       m_hSyncThreadAppHandle,  // Thread Application handle
                                       f_cNotification.c_str(),  // Notification
                                       SyncDataCallback<CFrameworkunifiedSyncData,
                                       &CFrameworkunifiedSyncData::FrameworkunifiedSyncDataNotifCallback>))) {  // Callback Function
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedSubscribeNotificationWithCallback  Sync Notification Data failed.");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "FrameworkunifiedSubscribeNotificationWithCallback Sync Notification Data success.");
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  pthread_spin_unlock(&m_pSyncLock);

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedUnSubscribeNotificationWithDataSync
/// This function is used to unsubscribe to notifications.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedUnSubscribeNotificationWithDataSync(const std::string &f_cNotification) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  pthread_spin_lock(&m_pSyncLock);
  if (TRUE != f_cNotification.empty() && NULL != m_hSyncThreadAppHandle) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedUnsubscribeNotificationWithCallback
                                     (m_hSyncThreadAppHandle,  // Thread Application Handle
                                      f_cNotification.c_str()))) {  // Notification
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "FrameworkunifiedUnsubscribeNotificationWithCallback failed.");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "FrameworkunifiedUnsubscribeNotificationWithCallback success.");

      TSyncDataPacketItr l_itSycDataPacketItr = m_mSyncDataMap->find(f_cNotification);
      // Delete the notification data
      if (m_mSyncDataMap->end() != l_itSycDataPacketItr) {
        SFrameworkunifiedSyncDataPacket *l_ptDataPacket = l_itSycDataPacketItr->second;
        if (NULL != l_ptDataPacket && NULL != l_ptDataPacket->m_pNotificationData) {
          delete[](static_cast<PCHAR>(l_ptDataPacket->m_pNotificationData));
          l_ptDataPacket->m_pNotificationData = NULL;
          delete l_ptDataPacket;
          l_ptDataPacket = NULL;
          m_mSyncDataMap->erase(l_itSycDataPacketItr);
        }
      }
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  pthread_spin_unlock(&m_pSyncLock);

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSyncDataNotifCallback
/// This function is callback to notifications on publication of notifications.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedSyncDataNotifCallback(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL == hThread) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  PCSTR l_pcLastNotification = FrameworkunifiedGetLastNotification(hThread);

  if (NULL == l_pcLastNotification || 0 == std::strlen(l_pcLastNotification)) {
    return eFrameworkunifiedStatusInvldNotification;
  }

  UI_32 l_uiBufferSize = FrameworkunifiedGetMsgLength(hThread);
  PVOID l_pDataBuffer = new(std::nothrow)CHAR[l_uiBufferSize];

  if ((NULL != l_pDataBuffer) &&
      (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hThread, l_pDataBuffer, l_uiBufferSize, eSMRRelease))) {
    SFrameworkunifiedSyncDataPacket *l_ptDataPacket = NULL;

    pthread_spin_lock(&m_pSyncLock);

    TSyncDataPacketItr l_itSycDataPacketItr = m_mSyncDataMap->find(l_pcLastNotification);
    // Delete the last notification data
    if (m_mSyncDataMap->end() == l_itSycDataPacketItr) {
      l_ptDataPacket = new(std::nothrow)SFrameworkunifiedSyncDataPacket();
    } else {
      l_ptDataPacket = l_itSycDataPacketItr->second;
      if (NULL != l_ptDataPacket && NULL != l_ptDataPacket->m_pNotificationData) {
        delete[](static_cast<PCHAR>(l_ptDataPacket->m_pNotificationData));
        l_ptDataPacket->m_pNotificationData = NULL;
      }
    }

    if (NULL != l_ptDataPacket) {
      l_ptDataPacket->m_pNotificationData = l_pDataBuffer;
      l_ptDataPacket->m_uiDataSize = l_uiBufferSize;
      l_ptDataPacket->m_tTimeStamp = time(NULL);
      m_mSyncDataMap->insert(std::make_pair(l_pcLastNotification, l_ptDataPacket));
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "NULL pointer: l_ptDataPacket");
    }

    pthread_spin_unlock(&m_pSyncLock);
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetSyncDataSize
/// This function is used to get the size of the synchronization data.
////////////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CFrameworkunifiedSyncData::FrameworkunifiedGetSyncDataSize(const std::string &f_cNotification) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  UI_32 l_uiSyncDataSize = 0;

  if (TRUE != f_cNotification.empty()) {
    pthread_spin_lock(&m_pSyncLock);

    TSyncDataPacketItr l_itSyncDataPacketItr = m_mSyncDataMap->find(f_cNotification);
    SFrameworkunifiedSyncDataPacket *l_ptDataPacket = NULL;

    if (m_mSyncDataMap->end() != l_itSyncDataPacketItr) {
      l_ptDataPacket = l_itSyncDataPacketItr->second;
      l_uiSyncDataSize = l_ptDataPacket->m_uiDataSize;
    }
    pthread_spin_unlock(&m_pSyncLock);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_uiSyncDataSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetSyncNotificationData
/// This function is used to get the synchronization notification data for a particular notification .
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CFrameworkunifiedSyncData::FrameworkunifiedGetSyncNotificationData(const std::string &f_cNotification, PVOID f_pBuffer,
                                                    UI_16 f_nBufferSize) {
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (TRUE != f_cNotification.empty() && NULL != f_pBuffer && 0 != f_nBufferSize) {
    pthread_spin_lock(&m_pSyncLock);
    TSyncDataPacketItr l_itSyncDataPacketItr = m_mSyncDataMap->find(f_cNotification);
    if (m_mSyncDataMap->end() != l_itSyncDataPacketItr) {
      if (NULL != l_itSyncDataPacketItr->second) {
        if (NULL != std::memcpy(f_pBuffer, l_itSyncDataPacketItr->second->m_pNotificationData,
                                l_itSyncDataPacketItr->second->m_uiDataSize)) {
          l_eStatus = eFrameworkunifiedStatusOK;
        } else {
          l_eStatus = eFrameworkunifiedStatusFail;
        }
      } else {
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }
    pthread_spin_unlock(&m_pSyncLock);
  } else {
    l_eStatus = eFrameworkunifiedStatusFail;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return l_eStatus;
}
