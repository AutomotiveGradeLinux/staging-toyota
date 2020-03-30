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
/// \brief    This file contains implementation of class CStateNorPersistenceNotification.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <native_service/ns_message_center_if.h>
#include <string>
#include <iostream>

#include "ns_npp_types.h"
#include "ns_npp_notificationpersistentservicelog.h"
#include "ns_npp_persistent_data.h"
#include "ns_npp_state_nor_persistence_notification.h"

HANDLE CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// CStateNorPersistenceNotification
/// Constructor of CStateNorPersistenceNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStateNorPersistenceNotification::CStateNorPersistenceNotification(const std::string &f_cnotificationname,
                                                                   const UI_32 f_uimaxmsgsize,
                                                                   const UI_32 f_uidelay,
                                                                   const EFrameworkunifiedPersistCategory f_epersistcategory):
  CStateNotification(f_cnotificationname, f_uimaxmsgsize),
  m_uiDelay(f_uidelay),
  m_ePersistCategory(f_epersistcategory) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_ePersistentType = eFrameworkunifiedImmediatePersistedStateVar;

  // also, register the notification with worker thread responsible for writing notification data
  if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {  // LCOV_EXCL_BR_LINE 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null  // NOLINT[whitespace/line_length]
    TImmediatePersistenceRegisterNotifInfo l_tRegisterNotifInfo = {};
    std::strncpy(l_tRegisterNotifInfo.m_cnotificationname,
                 f_cnotificationname.c_str(),
                 sizeof(l_tRegisterNotifInfo.m_cnotificationname) - 1);
    l_tRegisterNotifInfo.m_uidelay = f_uidelay;
    l_tRegisterNotifInfo.m_epersistcategory = f_epersistcategory;

    if (eFrameworkunifiedStatusOK != McSend(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread, AppName, NOR_PERSISTENCE_REGISTER, sizeof(l_tRegisterNotifInfo), &l_tRegisterNotifInfo)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend for cmd 0x%X failed", NOR_PERSISTENCE_REGISTER);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// ~CStateNorPersistenceNotification
/// Constructor of CStateNorPersistenceNotification class
////////////////////////////////////////////////////////////////////////////////////////////////////
CStateNorPersistenceNotification::~CStateNorPersistenceNotification() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // also, unregister the notification with worker thread responsible for writing notification data
  if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {  // LCOV_EXCL_BR_LINE 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null  // NOLINT[whitespace/line_length]
    TImmediatePersistenceUnregisterNotifInfo l_tUnregisterNotifInfo = {};
    std::strncpy(l_tUnregisterNotifInfo.m_cnotificationname,
                 GetNotificationName().c_str(),  // LCOV_EXCL_BR_LINE 11: except,C++ STL
                 sizeof(l_tUnregisterNotifInfo.m_cnotificationname) - 1);

    if (eFrameworkunifiedStatusOK != McSend(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread, AppName, NOR_PERSISTENCE_UNREGISTER, sizeof(l_tUnregisterNotifInfo), &l_tUnregisterNotifInfo)) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend for cmd 0x%X failed", NOR_PERSISTENCE_UNREGISTER);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistenceDelay
/// Method to get the persistence time delay.
////////////////////////////////////////////////////////////////////////////////////////////////////
UI_32 CStateNorPersistenceNotification::GetPersistenceDelay() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_uiDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Publish
/// This function publishes the notification to subscribed clients and saves the data
/// immediately to persistent memory.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNorPersistenceNotification::Publish(const std::string &f_cservicename,
                                                     PVOID f_pmessage,
                                                     const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (eFrameworkunifiedStatusOK == (l_estatus = PublishNotification(f_cservicename,
                                                       f_pmessage,
                                                       f_uimsgsize))) {
    if (eFrameworkunifiedStatusOK != (l_estatus = SaveDataToNor(f_pmessage, f_uimsgsize))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error in saving persistent data on nor for %s published by %s, status: %d",
             m_cNotificationName.c_str(), f_cservicename.c_str(), l_estatus);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// PublishNotification
/// This function publishes the notification to subscribed clients.
////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNorPersistenceNotification::PublishNotification(const std::string &f_cservicename,
                                                                 PVOID f_pmessage,
                                                                 const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  l_estatus = CStateNotification::Publish(f_cservicename, f_pmessage, f_uimsgsize);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SaveDataToNor
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNorPersistenceNotification::SaveDataToNor(PVOID f_pmessage, const UI_32 f_uimsgsize) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (NULL != f_pmessage && f_uimsgsize > 0) {
    TNorPersistenceNotifInfoHeader l_tNorPersistentData = {};
    UI_32 l_ui32SendDataTotalLength = static_cast<UI_32>(sizeof(TNorPersistenceNotifInfoHeader) + f_uimsgsize);
    UI_8 *l_ui8SendData = new(std::nothrow) UI_8[l_ui32SendDataTotalLength];
    UI_8 *l_ui8TmpOffset = l_ui8SendData;

    if (NULL != l_ui8SendData) {  // LCOV_EXCL_BR_LINE 6: l_ui8SendData can't be NULL
      std::strncpy(l_tNorPersistentData.m_cnotificationname,
                   this->GetNotificationName().c_str(),
                   sizeof(l_tNorPersistentData.m_cnotificationname) - 1);
      std::strncpy(l_tNorPersistentData.m_cpublishername,
                   this->GetPublisherName().c_str(),
                   sizeof(l_tNorPersistentData.m_cpublishername) - 1);
      l_tNorPersistentData.m_epersistenttype = this->GetNotificationType();
      l_tNorPersistentData.m_uimaxmsglength = this->GetMaxMessageSize();
      l_tNorPersistentData.m_uidelay = this->GetPersistenceDelay();
      l_tNorPersistentData.m_uimsgsize = f_uimsgsize;
      l_tNorPersistentData.m_epersistcategory = m_ePersistCategory;

      std::memset(l_ui8SendData, 0, l_ui32SendDataTotalLength);
      std::memcpy(l_ui8SendData, &l_tNorPersistentData, sizeof(l_tNorPersistentData));
      l_ui8TmpOffset += sizeof(l_tNorPersistentData);
      std::memcpy(l_ui8TmpOffset, f_pmessage, f_uimsgsize);

      // issue a copy to the worker thread
      if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {  // LCOV_EXCL_BR_LINE 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null  // NOLINT[whitespace/line_length]
        if (eFrameworkunifiedStatusOK != (l_estatus = McSend(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread, AppName, NOR_PERSISTENCE_TIMER_START, l_ui32SendDataTotalLength, l_ui8SendData))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "McSend for nor worker thread failed %d ", l_estatus);  // LCOV_EXCL_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
        }
      } else {
        // LCOV_EXCL_START 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        l_estatus = eFrameworkunifiedStatusInvldHandle;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Sender handle of nor worker thread is NULL. Can't send message.");
        // LCOV_EXCL_STOP
      }

      delete[] l_ui8SendData;  // LCOV_EXCL_BR_LINE 11: unexpected branch
      l_ui8SendData = NULL;
    } else {
      // LCOV_EXCL_START 6: l_ui8SendData can not be null
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_estatus = eFrameworkunifiedStatusNullPointer;
      // LCOV_EXCL_STOP
    }
  } else {
    l_estatus = eFrameworkunifiedStatusInvldParam;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Notification data is NULL, size is %d", f_uimsgsize);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GetPersistentCategory
/// Gets the persist type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedPersistCategory CStateNorPersistenceNotification::GetPersistentCategory() {   // LCOV_EXCL_START 100: never be used
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return m_ePersistCategory;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SetPersistentCategory
/// Sets the persist type of notification.
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CStateNorPersistenceNotification::SetPersistentCategory(const EFrameworkunifiedPersistCategory f_epersistcategory) {
  EFrameworkunifiedStatus l_estatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (f_epersistcategory != m_ePersistCategory) {
    if (NULL != m_pData && NULL != m_pData->m_pMessage) {  // LCOV_EXCL_BR_LINE 6: if m_pData is not null, the m_pData->m_pMessage can not be null  // NOLINT[whitespace/line_length]
      if (NULL != CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread) {  // LCOV_EXCL_BR_LINE 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null  // NOLINT[whitespace/line_length]
        TImmediatePersistenceChangeCategory l_tChangeCategory = {};
        UI_32 l_ui32SendDataTotalLength =
          static_cast<UI_32>(sizeof(TImmediatePersistenceChangeCategory) + m_pData->m_uiMsgSize);
        UI_8 *l_ui8SendData = new(std::nothrow) UI_8[l_ui32SendDataTotalLength];
        UI_8 *l_ui8TmpOffset = l_ui8SendData;

        if (NULL != l_ui8SendData) {  // LCOV_EXCL_BR_LINE 6: l_ui8SendData can't be NULL
          std::strncpy(l_tChangeCategory.m_tnornotifInfoheader.m_cnotificationname, this->GetNotificationName().c_str(),
                       sizeof(l_tChangeCategory.m_tnornotifInfoheader.m_cnotificationname) - 1);
          std::strncpy(l_tChangeCategory.m_tnornotifInfoheader.m_cpublishername, this->GetPublisherName().c_str(),
                       sizeof(l_tChangeCategory.m_tnornotifInfoheader.m_cpublishername) - 1);
          l_tChangeCategory.m_tnornotifInfoheader.m_epersistenttype = this->GetNotificationType();
          l_tChangeCategory.m_tnornotifInfoheader.m_uimaxmsglength = this->GetMaxMessageSize();
          l_tChangeCategory.m_tnornotifInfoheader.m_uidelay = this->GetPersistenceDelay();
          l_tChangeCategory.m_tnornotifInfoheader.m_uimsgsize = m_pData->m_uiMsgSize;
          l_tChangeCategory.m_tnornotifInfoheader.m_epersistcategory = f_epersistcategory;

          l_tChangeCategory.m_eoldpersistcategory = m_ePersistCategory;

          std::memset(l_ui8SendData, 0, l_ui32SendDataTotalLength);
          std::memcpy(l_ui8SendData, &l_tChangeCategory, sizeof(l_tChangeCategory));
          l_ui8TmpOffset += sizeof(l_tChangeCategory);
          std::memcpy(l_ui8TmpOffset, m_pData->m_pMessage, m_pData->m_uiMsgSize);

          // issue a copy to the worker thread
          if (eFrameworkunifiedStatusOK != (l_estatus = McSend(CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread,  // LCOV_EXCL_BR_LINE 4: NSFW error case  // NOLINT[whitespace/line_length]
                                                  AppName,
                                                  NOR_PERSISTENCE_CHANGE_CATEGORY,
                                                  l_ui32SendDataTotalLength,
                                                  l_ui8SendData))) {
            // LCOV_EXCL_START 4: NSFW error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "McSend failed while changing category for notfn %s from %d to %d, status=%d",
                   m_cNotificationName.c_str(), m_ePersistCategory, f_epersistcategory, l_estatus);
            // LCOV_EXCL_STOP
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_NPP_INFO, __FUNCTION__,
                   "Msg sent to immediate pers. thread to change persist category of %s from %d to %d",
                   m_cNotificationName.c_str(), m_ePersistCategory, f_epersistcategory);
          }

          delete[] l_ui8SendData;
          l_ui8SendData = NULL;
        } else {
          // LCOV_EXCL_START 6: l_ui8SendData can not be null
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Memory alloc error for l_ui8SendData, errno=%d", errno);
          l_estatus = eFrameworkunifiedStatusNullPointer;
          // LCOV_EXCL_STOP
        }
      } else {
        // LCOV_EXCL_START 6: CStateNorPersistenceNotification::m_hNSImmediatePersistenceThread can not be null
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Sender handle of nor worker thread is NULL. Can't send message.");
        l_estatus = eFrameworkunifiedStatusInvldHandle;
        // LCOV_EXCL_STOP
      }
    } else {  // no data is persisted in emmc for this notfn, so no need to send msg to worker thread
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Notfn %s has no data to persist", m_cNotificationName.c_str());
    }

    m_ePersistCategory = f_epersistcategory;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_estatus;
}
