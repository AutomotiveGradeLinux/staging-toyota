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
/// \ingroup  tag_SS_LoggerService
/// \brief    TODO
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_popups.h"
#include <string>
#include <vector>
CLoggerPopups::CLoggerPopups()
    : m_pLoggerCfg(NULL),
      m_startShown(FALSE) {
}

CLoggerPopups::~CLoggerPopups() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerPopups::Initialize(CLoggerCfg* f_pLoggerCfg) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  this->m_pLoggerCfg = f_pLoggerCfg;
  l_eStatus = (m_pLoggerCfg == NULL) ? eFrameworkunifiedStatusNullPointer : eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

BOOL CLoggerPopups::IsPopupNeeded(TLoggerErrorEvent f_event) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_ret = ((f_event.EventBitMask & EVENT_BIT_POPUP) != 0) ? TRUE : FALSE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

BOOL CLoggerPopups::CanShowStartPopup(
    std::vector<TLoggingArtifact>* f_remainingArtifact) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_ret = FALSE;
  if (f_remainingArtifact != NULL) {
    l_ret = TRUE;
    std::vector<TLoggingArtifact>::iterator l_iter;
    for (l_iter = f_remainingArtifact->begin();
        f_remainingArtifact->end() != l_iter; l_iter++) {
      if (l_iter->ArtifactId == eArtifactIdScreenShot) {
        l_ret = FALSE;
        break;
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

std::string CLoggerPopups::getDeviceName(std::string f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "";
  if (f_dest.length() == 0) {
  } else if (f_dest.find(this->m_pLoggerCfg->m_usb0MountPath) != std::string::npos) {
    l_ret = "USB 1";
  } else if (f_dest.find(this->m_pLoggerCfg->m_usb1MountPath) != std::string::npos) {
    l_ret = "USB 2";
  } else if (f_dest.find(this->m_pLoggerCfg->m_sdMountPath) != std::string::npos) {
    l_ret = "SD card";
  } else if (f_dest.find(this->m_pLoggerCfg->m_emmcOutputPath)
      != std::string::npos) {
    l_ret = "Emmc";
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. No matching Path found for %s",
           f_dest.c_str());
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %s", l_ret.c_str());
  return (l_ret);
}

EFrameworkunifiedStatus CLoggerPopups::ShowStartPopup(
    HANDLE f_hApp, TLoggerErrorEvent f_event,
    std::vector<TLoggingArtifact>* f_remainingArtifact, std::string f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  if (this->m_startShown != FALSE) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Pop up already shown");
  } else if (TRUE != IsPopupNeeded(f_event)) {
    this->m_startShown = TRUE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Start pop up not needed for event %d.",
           f_event.EventType);
  } else if (TRUE != CanShowStartPopup(f_remainingArtifact)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Screenshot still in artifact vector.");
  } else {
    this->m_startShown = TRUE;
    STLoggerFolderInfo l_folderInfo;
    l_folderInfo.FoldernameAndLogname[0] = '\0';
    std::string l_dest = this->getDeviceName(f_dest);
    std::size_t l_len = l_dest.copy(l_folderInfo.StorageTarget,
                                    sizeof(l_folderInfo.StorageTarget) - 1u, 0);
    l_folderInfo.StorageTarget[l_len] = '\0';
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info. Show start pop up. Destination: %s",
           l_folderInfo.StorageTarget);
    l_eStatus = FrameworkunifiedPublishEvent(f_hApp, SS_LOGGER_LOGSTARTED_EVT,
    NULL,
                                &l_folderInfo, sizeof(l_folderInfo));

    LOG_STATUS_IF_ERRORED(l_eStatus,
                          "FrameworkunifiedPublishEvent(SS_LOGGER_LOGSTARTED_EVT)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerPopups::ShowEndPopup(HANDLE f_hApp, TLoggerErrorEvent f_event,
                                       std::string f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  this->m_startShown = FALSE;
  if (TRUE != IsPopupNeeded(f_event)) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " Info. No pop up needed at end of Logging.");
  } else {
    STLoggerFolderInfo l_folderInfo;
    std::size_t l_len = f_dest.copy(
        l_folderInfo.FoldernameAndLogname,
        sizeof(l_folderInfo.FoldernameAndLogname) - 1u,
        f_dest.find_last_of('/') + 1);
    l_folderInfo.FoldernameAndLogname[l_len] = '\0';

    std::string l_dest = this->getDeviceName(f_dest);
    l_len = l_dest.copy(l_folderInfo.StorageTarget,
                        sizeof(l_folderInfo.StorageTarget) - 1, 0);
    l_folderInfo.StorageTarget[l_len] = '\0';
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " Info. Show end pop up. Filepath: %s, Device: %s",
           l_folderInfo.FoldernameAndLogname, l_folderInfo.StorageTarget);
    l_eStatus = FrameworkunifiedPublishEvent(f_hApp, SS_LOGGER_LOGINFO_EVT,
    NULL,
                                &l_folderInfo, sizeof(l_folderInfo));

    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedPublishEvent(SS_LOGGER_LOGINFO_EVT)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerPopups::ShowErrorPopup(HANDLE f_hApp,
                                         TLoggerErrorEvent f_event,
                                         UI_16 f_errCode) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_16 l_errorcode = (UI_16) f_errCode;
  this->m_startShown = FALSE;
  if (IsPopupNeeded(f_event) == TRUE) {
    l_eStatus = FrameworkunifiedPublishEvent(f_hApp, SS_LOGGER_ERRORINFO_EVT,
    NULL,
                                &l_errorcode, sizeof(l_errorcode));
    LOG_STATUS_IF_ERRORED(l_eStatus, "FrameworkunifiedPublishEvent(SS_LOGGER_ERRORINFO_EVT)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
