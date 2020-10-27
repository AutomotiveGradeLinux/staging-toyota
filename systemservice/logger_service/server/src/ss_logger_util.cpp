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
/// \ingroup  tag_SS_LoggerService
/// \brief    Provide support for SS Logger module utility functions.
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stub/Clock_API.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "ss_logger_common.h"

#include <system_service/ss_logger_store_logs.h>

#define SS_STORE_FILENAME_SYSILG_TERM "_SYS_ILG_RESET.tar.gz"
#define SS_STORE_FILENAME__CWORD52__TERM "__CWORD52_.log"


char CLoggerUtil::m_usbpath[USB_PATH_SIZE];

///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS
///////////////////////////////////////////////////////////////////////////////
CLoggerUtil::CLoggerUtil(void)
    : m_pCfg(NULL),
      m_Milage(0),
      m_bootCount(0) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_usbpath[0] = '\0';
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

CLoggerUtil::~CLoggerUtil(void) {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerUtil::Initialize(CLoggerCfg *f_pLoggerCfg) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  m_pCfg = f_pLoggerCfg;
  l_eStatus = (NULL != f_pLoggerCfg) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusInvldParam;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

std::string CLoggerUtil::getDestination(TLoggerErrorEvent &f_eventNtfData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+: %d ", f_eventNtfData.EventType);
  std::string l_path = "";
  EFrameworkunifiedStatus l_eStatus;

  switch (f_eventNtfData.EventType) {
    case eErrorEventTypeProcessCrash:
    case eErrorEventTypeHeartBeatFailure:
    case eErrorEventTypeSystemLowMemory:
    case eErrorEventTypeBootMicroReset:
    case eErrorEventTypeProcessExit:
    case eErrorEventTypeUserInvokedUserForceReset:
    case eErrorEventTypeModConnFailed:
    case eErrorEventTypeStartRespFailed:
    case eErrorEventTypeUserInvokedCollectDevLogs:
    case eErrorEventTypeModuleInvokedResetRequest:
    case eErrorEventTypeModuleInvokedCollectDebugLogs:
    case eErrorEventTypeDtcEvent:
    case eErrorEventTypeUserInvokedClearLogs:
    case eErrorEventTypeGroupRelaunch:
      l_eStatus = m_pCfg->GetEmmcDestination(l_path);
      break;

    case eErrorEventTypeUserInvokedCollectNaviLog:
      l_eStatus = m_pCfg->GetEmmcNaviLogDestination(l_path);
      break;

    case eErrorEventTypeEelExport:
      l_path = f_eventNtfData.ModuleName;
      l_eStatus = eFrameworkunifiedStatusOK;
      break;

    case eErrorEventTypeInterfaceunifiedEmmcLogs:
      l_path = f_eventNtfData.ModuleName;
      l_eStatus = eFrameworkunifiedStatusOK;
      break;

    case eErrorEventTypeUserInvokedCollectAllLogs:
    case eErrorEventTypeUserInvokedCollectScreenShot:
    case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
    case eErrorEventTypeCanEvent:
      l_eStatus = m_pCfg->GetUserInvokedDestination(l_path);
      break;

    case eErrorEventTypeDiagEvent:
      l_path = f_eventNtfData.ModuleName;
      l_eStatus = eFrameworkunifiedStatusOK;
      break;

    default:
      l_eStatus = eFrameworkunifiedStatusFail;
      break;
  }
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Could not get filepath ");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-: %s", l_path.c_str());
  return (l_path);
}
EFrameworkunifiedStatus CLoggerUtil::checkDestinationAvailable(
    TLoggerErrorEvent &f_eventNtfData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string l_path = CLoggerUtil::getDestination(f_eventNtfData);
  std::string::size_type ret;

  if (l_path.length() > 0) {
    ret = l_path.find(DEBUG_USB_PATH);
    if (ret != std::string::npos && ret == 0) {
      if (0 == access(m_usbpath, W_OK)) {  // LCOV_EXCL_BR_LINE 5:C code
        // LCOV_EXCL_START 5:C code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        std::string path = l_path.substr(sizeof(DEBUG_USB_PATH) - 1);
        f_eventNtfData.ModuleName = m_usbpath;
        f_eventNtfData.ModuleName.append(path);
        l_eStatus = eFrameworkunifiedStatusOK;
        // LCOV_EXCL_STOP
      }
    } else {
      if (0 == access(l_path.c_str(), W_OK)) {  // LCOV_EXCL_BR_LINE 5:C code
        l_eStatus = eFrameworkunifiedStatusOK;
      }
    }
  }

  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. Failed the check path: l_eStatus(%d), l_path(%s)",
           l_eStatus, l_path.c_str());  // LCOV_EXCL_BR_LINE 11:except,C++ STL
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

static void SetSuffixFromType(std::string &pathAndName, EErrorEventType type) {  // NOLINT (runtime/references)
  switch (type) {
    case eErrorEventTypeProcessCrash:
    case eErrorEventTypeHeartBeatFailure:
    case eErrorEventTypeSystemLowMemory:
    case eErrorEventTypeBootMicroReset:
    case eErrorEventTypeProcessExit:
    case eErrorEventTypeModConnFailed:
    case eErrorEventTypeStartRespFailed:
    case eErrorEventTypeModuleInvokedResetRequest:
      pathAndName.insert(pathAndName.find(".tar.gz"), "_ERR");
      break;
    case eErrorEventTypeUserInvokedUserForceReset:
    case eErrorEventTypeUserInvokedCollectAllLogs:
    case eErrorEventTypeUserInvokedCollectScreenShot:
    case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
    case eErrorEventTypeDiagEvent:
    case eErrorEventTypeCanEvent:
    case eErrorEventTypeDtcEvent:
    case eErrorEventTypeModuleInvokedCollectDebugLogs:
      pathAndName.insert(pathAndName.find(".tar.gz"), "_DIAG");
      break;
    case eErrorEventTypeUserInvokedCollectDevLogs:
      pathAndName.insert(pathAndName.find(".tar.gz"), "_DEBUG");
      break;
    case eErrorEventTypeGroupRelaunch:
      pathAndName.insert(pathAndName.find(".tar.gz"), "_GRP_RELAUNCH");
      break;
    default:
      break;
  }
}

EFrameworkunifiedStatus CLoggerUtil::getFilePathAndName(HANDLE f_hApp,
                                           TLoggerErrorEvent &f_eventNtfData,
                                           uint32_t f_time,
                                           std::string &f_pathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  f_pathAndName = "";

  switch (f_eventNtfData.EventType) {  // LCOV_EXCL_BR_LINE 200: eErrorEventTypeEelExport ex. can not be run
    case eErrorEventTypeProcessCrash:
    case eErrorEventTypeProcessExit:
    case eErrorEventTypeHeartBeatFailure:
    case eErrorEventTypeSystemLowMemory:
    case eErrorEventTypeBootMicroReset:
    case eErrorEventTypeModConnFailed:
    case eErrorEventTypeStartRespFailed:
    case eErrorEventTypeGroupRelaunch:
      l_eStatus = m_pCfg->GetEmmcDestination(f_pathAndName);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6:it will aways return ok
        // LCOV_EXCL_START 6:it will aways return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. GetEmmcDestination() returned %d.",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        f_pathAndName.append("/");
        f_pathAndName.append(GetEmmcFilename(f_time));
      }
      break;

    case eErrorEventTypeUserInvokedUserForceReset:
    case eErrorEventTypeUserInvokedCollectDevLogs:
      l_eStatus = m_pCfg->GetEmmcDestination(f_pathAndName);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6:it will aways return ok
        // LCOV_EXCL_START 6:it will aways return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. GetEmmcDestination() returned %d.",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        f_pathAndName.append("/");
        f_pathAndName.append(GetEmmcFilename(f_time));
      }
      break;
    case eErrorEventTypeModuleInvokedResetRequest:
    case eErrorEventTypeModuleInvokedCollectDebugLogs:
    case eErrorEventTypeDtcEvent:
      l_eStatus = m_pCfg->GetEmmcDestination(f_pathAndName);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6:it will aways return ok
        // LCOV_EXCL_START 6:it will aways return ok
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. GetEmmcDestination() returned %d.",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        f_pathAndName.append("/");
        f_pathAndName.append(GetEmmcFilename(f_time));
        if (!f_eventNtfData.ModuleName.empty()) {  // LCOV_EXCL_BR_LINE 6: ModuleName can not be empty
          size_t l_pos = f_pathAndName.find(".tar.gz");
          if (l_pos != std::string::npos) {  // LCOV_EXCL_BR_LINE 6: ".tar.gz" is aways exist
            f_pathAndName.insert(l_pos, f_eventNtfData.ModuleName);
          }
        }
      }
      break;
    case eErrorEventTypeEelExport:  // LCOV_EXCL_START 6: impossible to confirm because it can not get in
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_pathAndName = f_eventNtfData.ModuleName;
      f_pathAndName.append("/");
      f_pathAndName.append("loggerservicelogs_export_");
      f_pathAndName.append(GetTimeString(f_time));
      f_pathAndName.append(".tar.gz");
      break;
      // LCOV_EXCL_STOP
    case eErrorEventTypeInterfaceunifiedEmmcLogs:  // LCOV_EXCL_START 6: impossible to confirm because it can not get in
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      // store the target location on event trigger
      f_pathAndName = f_eventNtfData.ModuleName;
      break;
      // LCOV_EXCL_STOP
    case eErrorEventTypeUserInvokedCollectAllLogs:
      l_eStatus = m_pCfg->GetUserInvokedDestination(f_pathAndName);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: impossible to confirm because it can not be fail
        // LCOV_EXCL_START 6: impossible to confirm because it can not be fail
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. GetUserInvokedDestination() returned %d.",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        std::string l_emmc_path = m_pCfg->getEmmcOutputPath();
        f_pathAndName.append(l_emmc_path);
        f_pathAndName.append("/");
        f_pathAndName.append(GetUserInvokedFilename(f_hApp, f_time));
        if (!f_eventNtfData.ModuleName.empty()) {  // LCOV_EXCL_BR_LINE 6: it can not be empty
          size_t l_pos = f_pathAndName.find(".tar.gz");
          if (l_pos != std::string::npos) {  // LCOV_EXCL_BR_LINE 6: ".tar.gz" is aways exist
            f_pathAndName.insert(l_pos, f_eventNtfData.ModuleName);
          }
        }
      }
      break;

    case eErrorEventTypeUserInvokedCollectScreenShot:
    case eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs:
    case eErrorEventTypeCanEvent:
      l_eStatus = m_pCfg->GetUserInvokedDestination(f_pathAndName);
      if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 6: impossible to confirm because it can not be fail
        // LCOV_EXCL_START 6: impossible to confirm because it can not be fail
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. GetUserInvokedDestination() returned %d.",
               l_eStatus);
        // LCOV_EXCL_STOP
      } else {
        f_pathAndName.append("/");
        f_pathAndName.append(GetUserInvokedFilename(f_hApp, f_time));
      }
      break;

    case eErrorEventTypeDiagEvent:  // LCOV_EXCL_START 6: impossible to confirm because it can not get in
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_pathAndName = f_eventNtfData.ModuleName;
      f_pathAndName.append("/");
      f_pathAndName.append(GetUserInvokedFilename(f_hApp, f_time));
      l_eStatus = eFrameworkunifiedStatusOK;
      break;
      // LCOV_EXCL_STOP
    case eErrorEventTypeUserInvokedClearLogs:
    case eErrorEventTypeUserInvokedCollectNaviLog:
      l_eStatus = eFrameworkunifiedStatusOK;
      break;

    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Unknown event type: %d.",
             f_eventNtfData.EventType);
      l_eStatus = eFrameworkunifiedStatusFail;
      break;
  }

  SetSuffixFromType(f_pathAndName, f_eventNtfData.EventType);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

// LCOV_EXCL_START 13:
EFrameworkunifiedStatus CLoggerUtil::getNaviLogFilePathAndName(std::string &f_prefix,
                                                  uint32_t f_time,
                                                  std::string &f_pathAndName) {  // LCOV_EXCL_BR_LINE 13:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = m_pCfg->GetEmmcNaviLogDestination(f_pathAndName);
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. GetEmmNaviLogDestination() returned %d.",
           l_eStatus);
  } else {
    f_pathAndName.append("/");
    f_pathAndName.append(f_prefix);
    f_pathAndName.append(GetEmmcNaviLogFilename(f_time));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 200:
EFrameworkunifiedStatus CLoggerUtil::getEmmcNaviLogParams(
    uint32_t f_time, EPWR_SHUTDOWN_TRIGGER_TYPE errorType,
    std::string &f_pathName, UI_32 &f_logMax) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR *l_dirp = NULL;
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_prefix = "";

  std::vector<std::string> l_vector;
  std::string l_path = m_pCfg->getEmmcOutputNaviLogPath();

  if ((errorType == epssdmsdtGENERIC_ERROR_RESET)
      || (errorType == epssdmsdtFATAL_ERROR_RESET)) {
    /*When reset occured !!*/
    f_logMax = m_pCfg->m_emmcOutputNaviLogErrMax;
    l_prefix = "ERR_";
  } else {
    /* ACC OFF occured*/
    f_logMax = m_pCfg->m_emmcOutputNaviLogNmlMax;
    l_prefix = "NML_";
  }

  if (f_logMax == 0) {
    l_eStatus = eFrameworkunifiedStatusOK;
    return l_eStatus;
  }

  l_dirp = opendir(l_path.c_str());
  if (l_dirp != NULL) {
    struct dirent l_dirent;
    struct dirent* next;

    while (0 == readdir_r(l_dirp, &l_dirent, &next) && next != NULL) {
      std::string l_findFileName = l_dirent.d_name;
      size_t l_find_pos_top = l_findFileName.find(l_prefix.c_str());
      size_t l_find_pos = l_findFileName.find(".log");
      if (std::string::npos != l_find_pos
          && std::string::npos != l_find_pos_top) {
        l_vector.push_back(l_findFileName);
      }
    }
    closedir(l_dirp);
  }
  std::sort(l_vector.begin(), l_vector.end());

  for (UI_32 i = static_cast<UI_32>(l_vector.size()); i != 0; i--) {
    std::string l_FilePath = l_path;
    std::string l_FileName = l_vector.at(i - 1);
    l_FilePath.append("/");
    l_FilePath.append(l_FileName.c_str());

    if (i < f_logMax) {
      size_t l_pos = l_FileName.find("_", l_prefix.size());
      if (std::string::npos != l_pos) {
        std::string l_renameFilePath = l_path;
        std::stringstream l_numStream;
        l_numStream << std::setw(5) << std::setfill('0') << std::right << std::dec << (i + 1);
        l_renameFilePath.append("/");
        l_renameFilePath.append(l_prefix);
        l_renameFilePath.append(l_numStream.str());
        l_renameFilePath.append(l_FileName, l_pos, (l_FileName.size() - l_pos));
        if (0 != rename(l_FilePath.c_str(), l_renameFilePath.c_str())) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. rename navilog file %s:%d.",
                 l_FilePath.c_str(), errno);
        }
      }
    } else {
      if (0 != unlink(l_FilePath.c_str())) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete navilog file %s:%d.",
               l_FilePath.c_str(), errno);
      }
    }
  }
  // sync parent directory
  SyncDir(l_path);

  l_eStatus = getNaviLogFilePathAndName(l_prefix, f_time, f_pathName);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 200: can not be called
size_t CLoggerUtil::deleteRequestLogs(std::string f_emmcpath,
                                      ELOGGERSERVICELOGTYPE f_reqtype, size_t f_reqsize,
                                      ELOGGERSERVICELOGTYPE f_deltype) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR* l_dirp = NULL;
  std::vector<std::string> vect;
  std::vector<std::string>::iterator it;
  UI_32 l_count = 0;
  size_t l_totalsize = 0;

  // create saved file list on eMMC
  l_dirp = opendir(f_emmcpath.c_str());
  if (l_dirp != NULL) {
    struct dirent l_dirent;
    struct dirent* next;
    while (0 == readdir_r(l_dirp, &l_dirent, &next) && next != NULL) {
      std::string l_filename = l_dirent.d_name;
      if (CLoggerUtil::QueryLoggerservicelogType(l_filename) == f_deltype) {
        vect.push_back(l_filename);
      }
    }
    closedir(l_dirp);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " open failed eMMC path: %s",
           f_emmcpath.c_str());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return 0;
  }

  // get num of delete logs
  std::sort(vect.begin(), vect.end());
  for (it = vect.begin(); it != vect.end(); it++) {
    l_count++;

    std::string l_filename = *it;
    struct stat st_buf;
    std::string l_tmpstr = f_emmcpath;
    l_tmpstr.append(l_filename.c_str());
    if (stat(l_tmpstr.c_str(), &st_buf) != -1) {
      l_totalsize += st_buf.st_size;
    }

    if (l_totalsize > f_reqsize) {
      break;
    }
  }

  // start delete logs
  if ((l_totalsize > f_reqsize) || (f_reqtype == eLoggerservicelogTypeAbnormal)
      || (f_reqtype == eLoggerservicelogTypeGrpRelaunch)) {
    for (it = vect.begin(); it != vect.end() && l_count > 0; it++) {
      l_count--;

      std::string l_filename = *it;
      std::string l_tmpstr = f_emmcpath;
      l_tmpstr.append(l_filename.c_str());
      if (unlink(l_tmpstr.c_str()) != 0) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " unlink failed %s:%d.",
               l_tmpstr.c_str(), errno);
      }
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " del-file=%s total=%d", l_tmpstr.c_str(),
             l_totalsize);
    }
    SyncDir(f_emmcpath);
  } else {
    l_totalsize = 0;
    FRAMEWORKUNIFIEDLOG(
        ZONE_INFO,
        __FUNCTION__,
        " nothing to delete logs req-type=%d del-logtype=%d total=%d, log_count=%d",
        f_reqtype, f_deltype, l_totalsize, l_count);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_totalsize;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 200: can not be called
EFrameworkunifiedStatus CLoggerUtil::forceDeleteOldLog(std::string f_archive_destination,
                                          size_t f_req_size) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string l_emmc_path;
  size_t l_delsize = 0;
  ELOGGERSERVICELOGTYPE l_logtype = eLoggerservicelogTypeOther;

  size_t l_pos = f_archive_destination.find_last_of('/');
  if (std::string::npos != l_pos) {
    l_emmc_path = f_archive_destination.substr(0, l_pos);
    l_emmc_path.append("/");
    l_logtype = CLoggerUtil::QueryLoggerservicelogType(
        f_archive_destination.substr(l_pos + 1,
                                     f_archive_destination.size() - l_pos + 1));
  }

  if (l_logtype == eLoggerservicelogTypeOther) {
    // not found
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Invalid Parameter. dest=%s",
           f_archive_destination.c_str());
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
    return eFrameworkunifiedStatusFail;
  }

  // remove dirty-file if exist
  if (unlink(f_archive_destination.c_str()) != 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " unlink failed %s:%d.",
           f_archive_destination.c_str(), errno);
  }

  // delete normal log
  l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, f_req_size,
                                eLoggerservicelogTypeNormal);
  if (l_delsize >= f_req_size) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " delete normal logs success: req-size=%d, del-size=%d", f_req_size,
           l_delsize);
    l_eStatus = eFrameworkunifiedStatusOK;
  } else if (l_logtype == eLoggerservicelogTypeAbnormal) {
  // delete abnormal log if request type is abnormal
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " continue delete abnormal logs: req-size=%d, del-size=%d",
           f_req_size, l_delsize);
    size_t l_tmpsize = f_req_size;
    l_tmpsize -= l_delsize;
    l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, l_tmpsize,
                                  eLoggerservicelogTypeAbnormal);
    if (l_delsize >= l_tmpsize) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " delete abnormal logs success: req-size=%d, del-size=%d",
             f_req_size, l_delsize);
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " abnormal log delete not complete. req-size=%d, del-size=%d",
             f_req_size, l_delsize);
    }
  } else if (l_logtype == eLoggerservicelogTypeGrpRelaunch) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           " continue delete group relaunch logs: req-size=%d, del-size=%d",
           f_req_size, l_delsize);
    size_t l_tmpsize = f_req_size;
    l_tmpsize -= l_delsize;
    l_delsize = deleteRequestLogs(l_emmc_path, l_logtype, l_tmpsize,
                                  eLoggerservicelogTypeGrpRelaunch);
    if (l_delsize >= l_tmpsize) {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
             " delete group relaunch logs success: req-size=%d, del-size=%d",
             f_req_size, l_delsize);
      l_eStatus = eFrameworkunifiedStatusOK;
    } else {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          " group relaunch log delete not complete. req-size=%d, del-size=%d",
          f_req_size, l_delsize);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " normal log del not complete. req-size=%d, del-size=%d", f_req_size,
           l_delsize);
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerUtil::deleteOldLog(std::string log_path,
                                     std::vector<std::string>& l_vector,
                                     std::string& f_archive_destination,
                                     UI_32 max_num) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  size_t l_pos = f_archive_destination.find_last_of('/');
  if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 200: there is always '/' in f_destination
    std::sort(l_vector.begin(), l_vector.end());
    std::vector<std::string>::iterator itr = l_vector.begin();
    for (UI_32 i = static_cast<UI_32>(l_vector.size()); (max_num - 1) < i; i--, itr++) {
      if (itr == l_vector.end()) {  // LCOV_EXCL_BR_LINE 200: (max_num - 1) can not be -1.
        break;
      }
      std::string l_FilePath = log_path;
      std::string l_FileName = *itr;
      l_FilePath.append("/");
      l_FilePath.append(l_FileName.c_str());
      if (0 != unlink(l_FilePath.c_str())) {
        l_eStatus = eFrameworkunifiedStatusAccessError;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete old log file %s:%d.",
               l_FilePath.c_str(), errno);
      }
    }
    SyncDir(log_path);
  } else {
    // LCOV_EXCL_START 200: there is always '/' in f_destination
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusFail;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. delete target not found. %s",
           f_archive_destination.c_str());
    // LCOV_EXCL_STOP
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus CLoggerUtil::deleteOldEmmcLog(std::string& f_archive_destination,
                                         EErrorEventType type) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  DIR *l_dirp = NULL;
  std::vector<std::string> vect_abnrm; /* ILLEGAL RESET LOG INFO or ERROR EXIT LOG INFO */
  std::vector<std::string> vect_nrm; /* NORMAL LOG INFO */
  std::vector<std::string> vect_rlnc; /* RELAUNCH LOG INFO */
  std::string l_emmc_path = m_pCfg->getEmmcOutputPath();
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  UI_32 abnrm_count = 0;

  l_dirp = opendir(l_emmc_path.c_str());
  if (l_dirp != NULL) {  // LCOV_EXCL_BR_LINE 5: opendir's error case.
    struct dirent l_dirent;
    struct dirent* next;
    while (0 == readdir_r(l_dirp, &l_dirent, &next) && next != NULL) {
      std::string l_findFileName = l_dirent.d_name;

//      if ((std::string::npos != l_findFileName.find("_ILG_RESET.tar.gz"))
//          || (std::string::npos != l_findFileName.find("_ERR.tar.gz"))) {
//        vect_abnrm.push_back(l_findFileName);
      if ((l_findFileName == ".") || (l_findFileName == "..")) {
        continue;
      } else if (std::string::npos != l_findFileName.find(SS_STORE_FILENAME_SYSILG_TERM)) {
        vect_abnrm.push_back(l_findFileName);
      } else if(std::string::npos != l_findFileName.find(SS_STORE_FILENAME__CWORD52__TERM)) {  // LCOV_EXCL_BR_LINE 8: SS_STORE_FILENAME__CWORD52__TERM can not be set  // NOLINT[whitespace/line_length]
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        vect_abnrm.push_back(l_findFileName);  // LCOV_EXCL_LINE 8: SS_STORE_FILENAME_SYSILG_TERM can not be set  // NOLINT[whitespace/line_length]
      } else if((std::string::npos != l_findFileName.find("_ILG_RESET.tar.gz")) ||
                (std::string::npos != l_findFileName.find("_ERR.tar.gz"))) {
        vect_abnrm.push_back(l_findFileName);
        abnrm_count++;

      } else if (std::string::npos
          != l_findFileName.find("_GRP_RELAUNCH.tar.gz")) {
        vect_rlnc.push_back(l_findFileName);
      } else if (std::string::npos != l_findFileName.find(".tar.gz")) {
        vect_nrm.push_back(l_findFileName);
      }
    }
    closedir(l_dirp);
  }

  switch (type) {
    case eErrorEventTypeProcessCrash:
    case eErrorEventTypeProcessExit:
    case eErrorEventTypeHeartBeatFailure:
    case eErrorEventTypeSystemLowMemory:
    case eErrorEventTypeBootMicroReset:
    case eErrorEventTypeModConnFailed:
    case eErrorEventTypeStartRespFailed:
    case eErrorEventTypeModuleInvokedResetRequest:

//      if ((UI_32) m_pCfg->m_emmcOutputErrMax <= vect_abnrm.size()) {
//        l_eStatus = deleteOldLog(l_emmc_path, vect_abnrm, f_archive_destination,
//                                 (UI_32) m_pCfg->m_emmcOutputErrMax);
      if ((UI_32)m_pCfg->m_emmcOutputErrMax <= abnrm_count) {
        l_eStatus = SS_LoggerStoreLogs_deleteOldLogAbnrm(l_emmc_path, vect_abnrm, f_archive_destination,
                                                         (UI_32)m_pCfg->m_emmcOutputErrMax, abnrm_count);

      }
      break;
    case eErrorEventTypeGroupRelaunch:
      if ((UI_32) m_pCfg->m_emmcOutputGrpRelaunchMax <= vect_rlnc.size()) {
        l_eStatus = deleteOldLog(l_emmc_path, vect_rlnc, f_archive_destination,
                                 (UI_32) m_pCfg->m_emmcOutputGrpRelaunchMax);
      }
      break;
    default:
      if ((UI_32) m_pCfg->m_emmcOutputMax <= vect_nrm.size()) {
        l_eStatus = deleteOldLog(l_emmc_path, vect_nrm, f_archive_destination,
                                 (UI_32) m_pCfg->m_emmcOutputMax);
      }
      break;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

void CLoggerUtil::SetMilage(UI_32 f_Milage) {
  m_Milage = f_Milage;
}

std::string CLoggerUtil::GetUserInvokedFilename(HANDLE f_hApp, uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "";
  std::stringstream l_nameStream;
  UI_32 l_seqNumber = 0;

  EFrameworkunifiedStatus l_eStatus = m_pCfg->GetIncrementAndPersistUserInvokedCounter(
      f_hApp, l_seqNumber);
  LOG_STATUS_IF_ERRORED(
      l_eStatus,
      "GetIncrementAndPersistUserInvokedCounter()");

  /* new counter value was not persisted if l_eStatus != eFrameworkunifiedStatusOK */

  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec << l_seqNumber << '_';
  l_nameStream << std::setw(8) << std::setfill('0') << std::right << std::dec << m_Milage << '_';
  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec << m_bootCount << '_';
  l_nameStream << std::setw(15) << GetTimeString(f_time) << ".tar.gz";

  l_ret = l_nameStream.str();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

std::string CLoggerUtil::GetEmmcFilename(uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "";
  std::stringstream l_nameStream;
  UI_32 l_counter = 0;

  m_pCfg->GetAndIncrementEmmcCounter(l_counter);
  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec << l_counter << '_';
  l_nameStream << std::setw(15) << this->GetTimeString(f_time) << ".tar.gz";

  l_ret = l_nameStream.str();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

// LCOV_EXCL_START 8: dead code
std::string CLoggerUtil::GetEmmcErrorFilename(uint32_t f_time) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "";
  std::stringstream l_nameStream;
  UI_32 l_counter = 0;

  m_pCfg->GetAndIncrementEmmcCounter(l_counter);
  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec << l_counter << '_';
  l_nameStream << std::setw(15) << this->GetTimeString(f_time) << "_ERR.tar.gz";

  l_ret = l_nameStream.str();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 13:
std::string CLoggerUtil::GetEmmcNaviLogFilename(uint32_t f_time) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "";
  std::stringstream l_nameStream;
  UI_32 l_counter = 1;

  l_nameStream << std::setw(5) << std::setfill('0') << std::right << std::dec << l_counter << '_';
  l_nameStream << std::setw(15) << this->GetTimeString(f_time) << ".log";

  l_ret = l_nameStream.str();

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}
// LCOV_EXCL_STOP

std::string CLoggerUtil::GetTimeString(uint32_t f_time) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::string l_ret = "00000000_000000";
  struct tm local_time;
  Clock_getLocalTimeY2K38(&f_time, &local_time);
  char buffer[20];
  if (0
      != strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S",  // LCOV_EXCL_BR_LINE 5: c code
                  &local_time)) {
    l_ret = buffer;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_ret);
}

void CLoggerUtil::SetUsbMountPath(std::string usbpath) {
  strncpy(m_usbpath, usbpath.c_str(), USB_PATH_SIZE - 1);
  m_usbpath[USB_PATH_SIZE - 1] = '\0';
}
#ifdef RELEASE_BUILD
EFrameworkunifiedStatus CLoggerUtil::PathCheckAndCopyFile(std::string f_source,
    std::string f_destination) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  std::string::size_type ret;

  ret = f_destination.find(m_usbpath);
  l_eStatus = CopyFile(f_source, f_destination);

  return l_eStatus;
}
#endif  // RELEASE_BUILD
/////////////////////////////////////////////////////////////////////////
///// Static Functions
/////////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus CLoggerUtil::CopyFile(std::string f_source,
                                 std::string f_destination) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  ssize_t l_nrd;
  ssize_t l_nwr;
  char l_buf[BUF_MAX_SIZE];

  int l_if = open(f_source.c_str(), O_RDONLY | O_CLOEXEC, 0);
  if (-1 == l_if) {  // LCOV_EXCL_BR_LINE 5: open's error case.
    // LCOV_EXCL_START 5: open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "src open fail:%s,errno:%d,msg:%s",
           f_source.c_str(), errno, strerror(errno));
    l_eStatus = eFrameworkunifiedStatusFail;
    // LCOV_EXCL_STOP
  } else {
    bool isWebdav = false;
    std::string l_tmpDest;
    size_t l_pos = f_destination.find_last_of('/');

    l_tmpDest = f_destination;
    if (f_destination.find("/mnt/import") != std::string::npos) {  // LCOV_EXCL_BR_LINE 13:
      // if output to webdav's directory, don't create temporary file.
      // because of being unable to rename the file on it.
      isWebdav = true;
    }

    int l_of = open(l_tmpDest.c_str(), O_WRONLY | O_CREAT | O_CLOEXEC, 0640);
    if (-1 == l_of) {  // LCOV_EXCL_BR_LINE 5: open's error case.
      // LCOV_EXCL_START 5: open's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "dest open fail:%s,errno:%d,msg:%s",
             l_tmpDest.c_str(), errno, strerror(errno));
      l_eStatus = eFrameworkunifiedStatusFail;
      // LCOV_EXCL_STOP
    } else {
      do {
        l_nrd = read(l_if, l_buf, sizeof(l_buf));
        if (l_nrd == -1) {  // LCOV_EXCL_BR_LINE 5: read's error case.
          // LCOV_EXCL_START 5: read's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read fail:%s,errno:%d,msg:%s",
                 f_source.c_str(), errno, strerror(errno));
          l_eStatus = eFrameworkunifiedStatusAccessError;
          break;
          // LCOV_EXCL_STOP
        }

        l_nwr = write(l_of, l_buf, l_nrd);
        if (l_nwr == -1) {  // LCOV_EXCL_BR_LINE 5: write's error case.
          // LCOV_EXCL_START 5: write's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          if (ENOSPC == errno) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   "write fail(ENOSPC):%s,errno:%d,msg:%s", l_tmpDest.c_str(),
                   errno, strerror(errno));
            l_eStatus = eFrameworkunifiedStatusErrNoEAGAIN;
          } else {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "write fail:%s,errno:%d,msg%s",
                   l_tmpDest.c_str(), errno, strerror(errno));
            l_eStatus = eFrameworkunifiedStatusAccessError;
          }
          break;
          // LCOV_EXCL_STOP
        }
      } while (l_nrd > 0);

      fsync(l_of);
      close(l_of);

      if (l_eStatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 6: can not be fail
        // LCOV_EXCL_START 6: can not be fail
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        close(l_if);
        return l_eStatus;
        // LCOV_EXCL_STOP
      }

      {
        struct stat istat_buf, ostat_buf;
        int ret;
        ret = fstat(l_if, &istat_buf);
        if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case.
          // LCOV_EXCL_START 5: fstat's error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. stat(%s). errno: %d, %s",
                 f_source.c_str(), errno, strerror(errno));
          // LCOV_EXCL_STOP
        } else {
          while (1) {
            ret = stat(l_tmpDest.c_str(), &ostat_buf);
            if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: stat's error case.
              // LCOV_EXCL_START 5: stat's error case.
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. stat(%s). errno: %d, %s",
                     l_tmpDest.c_str(), errno, strerror(errno));
              break;
              // LCOV_EXCL_STOP
            }
            if (ostat_buf.st_size >= istat_buf.st_size) {  // LCOV_EXCL_BR_LINE 5: read and write can not fail, so the size is same  // NOLINT[whitespace/line_length]
              break;
            }
            usleep(10000);  // interval
          }
        }
      }
    }
    close(l_if);

    if (!isWebdav) {  // LCOV_EXCL_BR_LINE 6: isWebdav is aways false
      if (rename(l_tmpDest.c_str(), f_destination.c_str()) != 0) {  // LCOV_EXCL_BR_LINE 5: rename's error case.
        // LCOV_EXCL_START 5: rename's error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "rename %s to %s fail,errno:%d,msg:%s",
               l_tmpDest.c_str(), f_destination.c_str(), errno, strerror(errno));
        // LCOV_EXCL_STOP
      }
    }

    if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 200: there is always '/' in f_destination
      std::string l_destPath = f_destination.substr(0, l_pos);
      SyncDir(l_destPath);
    } else {
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "dest invalid:%s", f_destination.c_str());  // LCOV_EXCL_LINE 200: there is always '/' in f_destination  // NOLINT[whitespace/line_length]
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CLoggerUtil::CopyUntyped(std::string f_source,
                                    std::string f_destination) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  struct stat l_stat;
  int l_statRetVal;
  if (0 != access(f_source.c_str(), F_OK)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. The specified file path and name does not exist: %s",
           f_source.c_str());
  } else if (0 != (l_statRetVal = lstat(f_source.c_str(), &l_stat))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. stat() for file or directory returned error: %s -> %d",
           f_source.c_str(), l_statRetVal);
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    l_eStatus = CopyDirectory(f_source, f_destination);
  } else if (0 != S_ISLNK(l_stat.st_mode)) {
      // avoid duplicate copy for eErrorEventTypeInterfaceunifiedEmmcLogs: /nv/log/awlog -> /nv/BS/ss/logger_service/rwdata/log2/awlog
      if (f_source.find("/nv/BS/ss/logger_service/rwdata/awlog") == std::string::npos) {
        l_eStatus = CopyDirectory(f_source, f_destination);
      }
  } else if (0 != S_ISREG(l_stat.st_mode)) {
#ifdef RELEASE_BUILD
    l_eStatus = PathCheckAndCopyFile(f_source, f_destination);
#else
    l_eStatus = CopyFile(f_source, f_destination);
#endif  // RELEASE_BUILD
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. specified file may be a symbolic link or other non 'regular' type. File: %s, mode: 0x%04X.",
        f_source.c_str(), l_stat.st_mode);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CLoggerUtil::CopyDirectory(std::string f_source,
                                      std::string f_destination) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  DIR * l_pDir = opendir(f_source.c_str());

  if (l_pDir == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s",
           f_source.c_str());
  } else {
    if (mkdir(f_destination.c_str(), 0775) == -1) {
      if (errno == EEXIST) {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Directory: %s already exists.",
               f_destination.c_str());
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Could not create directory: %s. Errno: %s",
               f_destination.c_str(), strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
      }
    } else {
      SyncDir(f_destination);
      size_t l_pos = f_destination.find_last_of('/');
      if (l_pos != std::string::npos) {
        std::string l_destPath = f_destination.substr(0, l_pos);
        SyncDir(l_destPath);
      }
    }

    struct dirent l_pDirent;
    struct dirent* next;
    while (0 == readdir_r(l_pDir, &l_pDirent, &next) && next != NULL) {
      if ((0 != strcmp(l_pDirent.d_name, ".")) && /* Ignore special  . directory.                     */
      (0 != strcmp(l_pDirent.d_name, "..")) && /* Ignore special .. directory.                     */
      (0 != strcmp(l_pDirent.d_name, "lost+found")) && /* Ignore lost+found.                     */
      ('.' != l_pDirent.d_name[0])) {  /* Ignore hidden files                              */
        std::string l_fileSource = f_source;
        std::string l_fileDestination = f_destination;
        l_fileSource.append("/");
        l_fileSource.append(l_pDirent.d_name);
        l_fileDestination.append("/");
        l_fileDestination.append(l_pDirent.d_name);
        l_eStatus = CopyUntyped(l_fileSource, l_fileDestination);
      }
    }

    closedir(l_pDir);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CLoggerUtil::MoveFile(std::string f_source,
                                 std::string f_destination) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
#ifdef RELEASE_BUILD
  l_eStatus = CLoggerUtil::PathCheckAndCopyFile(f_source, f_destination);
  LOG_STATUS_IF_ERRORED(l_eStatus, "CLoggerUtil::PathCheckAndCopyFile()");
#else
  l_eStatus = CLoggerUtil::CopyFile(f_source, f_destination);
  LOG_STATUS_IF_ERRORED(l_eStatus, "CLoggerUtil::CopyFile()");
#endif  // RELEASE_BUILD
  if (eFrameworkunifiedStatusOK == l_eStatus) {
    l_eStatus = (0 == remove(f_source.c_str())) ? eFrameworkunifiedStatusOK : eFrameworkunifiedStatusFail;
    if (l_eStatus == eFrameworkunifiedStatusOK) {
      size_t l_pos = f_source.find_last_of('/');
      if (std::string::npos != l_pos) {
        std::string l_emmc_path = f_source.substr(0, l_pos);
        SyncDir(l_emmc_path);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CLoggerUtil::MoveUntyped(std::string f_source,
                                    std::string f_destination) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusFail;
  struct stat l_stat;
  int l_statRetVal;
  if (0 != access(f_source.c_str(), F_OK)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. The specified file path and name does not exist: %s",
           f_source.c_str());
  } else if (0 != (l_statRetVal = stat(f_source.c_str(), &l_stat))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           " Error. stat() for file or directory returned error: %s -> %d",
           f_source.c_str(), l_statRetVal);
  } else if (0 != S_ISDIR(l_stat.st_mode)) {
    l_eStatus = CLoggerUtil::MoveDirectory(f_source, f_destination);
    LOG_STATUS_IF_ERRORED(
        l_eStatus, "CLoggerUtil::MoveDirectory(f_source, f_destination);");
  } else if (0 != S_ISREG(l_stat.st_mode)) {
    l_eStatus = CLoggerUtil::MoveFile(f_source, f_destination);
    LOG_STATUS_IF_ERRORED(l_eStatus,
                          "CLoggerUtil::MoveFile(f_source, f_destination)");
  } else {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. specified file may be a symbolic link or other non 'regular' type. File: %s, mode: 0x%04X.",
        f_source.c_str(), l_stat.st_mode);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 8: can not be called
EFrameworkunifiedStatus CLoggerUtil::MoveDirectory(std::string f_source,
                                      std::string f_destination) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  DIR * l_pDir = opendir(f_source.c_str());

  if (l_pDir == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Could not open directory: %s",
           f_source.c_str());
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    errno = 0;
    if (mkdir(f_destination.c_str(), 0666) == -1) {
      if (errno == EEXIST) {
        FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Directory: %s already exists.",
               f_destination.c_str());
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "Could not create directory: %s. Errno: %s",
               f_destination.c_str(), strerror(errno));
        l_eStatus = eFrameworkunifiedStatusFail;
      }
      errno = 0;
    } else {
      SyncDir(f_destination);
      size_t l_pos = f_destination.find_last_of('/');
      if (l_pos != std::string::npos) {
        std::string l_destPath = f_destination.substr(0, l_pos);
        SyncDir(l_destPath);
      }
    }

    if (eFrameworkunifiedStatusOK == l_eStatus) {  /* if directory already exists or has been newly created */
      struct dirent l_pDirent;
      struct dirent* next;

      while (0 == readdir_r(l_pDir, &l_pDirent, &next) && next != NULL) {
        if ((0 != strcmp(l_pDirent.d_name, ".")) && /* Ignore special  . directory.                     */
        (0 != strcmp(l_pDirent.d_name, "..")) && /* Ignore special .. directory.                     */
        (0 != strcmp(l_pDirent.d_name, "lost+found")) && /* Ignore lost+found.                     */
        ('.' != l_pDirent.d_name[0])) {  /* Ignore hidden files                              */
          std::string l_fileSource = f_source;
          std::string l_fileDestination = f_destination;
          l_fileSource.append("/");
          l_fileSource.append(l_pDirent.d_name);
          l_fileDestination.append("/");
          l_fileDestination.append(l_pDirent.d_name);
          l_eStatus = MoveUntyped(l_fileSource, l_fileDestination);
        }
      }
    } else {
    }
    closedir(l_pDir);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START 200: can not be called
ELOGGERSERVICELOGTYPE CLoggerUtil::QueryLoggerservicelogType(std::string f_logname) {
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
//  if (f_logname.find("_ILG_RESET.tar.gz") != std::string::npos
  if (f_logname.find(SS_STORE_FILENAME_SYSILG_TERM) != std::string::npos) {
    return eLoggerservicelogTypeAbnormal;
  } else if (f_logname.find(SS_STORE_FILENAME__CWORD52__TERM) != std::string::npos) {
    return eLoggerservicelogTypeAbnormal;
  } else if (f_logname.find("_ILG_RESET.tar.gz") != std::string::npos

      || f_logname.find("_ERR.tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeAbnormal;
  } else if (f_logname.find("_GRP_RELAUNCH.tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeGrpRelaunch;
  } else if (f_logname.find(".tar.gz") != std::string::npos) {
    return eLoggerservicelogTypeNormal;
  } else {
    return eLoggerservicelogTypeOther;
  }
}
// LCOV_EXCL_STOP

void CLoggerUtil::SyncDir(std::string f_dir_path) {
  int fd = open(f_dir_path.c_str(), O_RDONLY | O_DIRECTORY | O_CLOEXEC);
#define OPE_ERR_MSG "%s %s failed. errno:%d,msg:%s"
  if (fd == -1) {  // LCOV_EXCL_BR_LINE 5: open's error case.
    // LCOV_EXCL_START 5: open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "open",
           errno, strerror(errno));
    // LCOV_EXCL_STOP
  } else {
    if (fsync(fd) == -1) {  // LCOV_EXCL_BR_LINE 5: open's error case.
    // LCOV_EXCL_START 5: open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "fsync",
             errno, strerror(errno));
    // LCOV_EXCL_STOP
    }
    if (close(fd) == -1) {  // LCOV_EXCL_BR_LINE 5: open's error case.
    // LCOV_EXCL_START 5: open's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, OPE_ERR_MSG, f_dir_path.c_str(), "close",
             errno, strerror(errno));
    // LCOV_EXCL_STOP
    }
  }
#undef OPE_ERR_MSG
}

