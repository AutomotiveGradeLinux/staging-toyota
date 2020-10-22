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
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_UTIL_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_UTIL_H_

///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <native_service/frameworkunified_types.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_system_manager_if.h>
#include <string>
#include <vector>
#include "ss_logger_error_event_cfg.h"
#include "loggerservicedebug_loggerservicelog.h"
#include "ss_logger_cfg.h"
///////////////////////////////////////////////////////////////////////////////
// DEFINE
///////////////////////////////////////////////////////////////////////////////
// If you changed BUF_MAX_SIZE, To contact the secure team.
// Because There is impact for decrypt tool.
#define BUF_MAX_SIZE (1024*8)
#define USB_PATH_SIZE 64
#ifdef RELEASE_BUILD
#define DEV_ENC_EXTENSION            ".enc1"
#define REL_ENC_EXTENSION            ".enc2"
#define ALIGNMENT_SIZE 4
#endif  // RELEASE_BUILD
#define LOGGER_TMP_FN                "__LOGGER.tmp"

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION CLASS DEFINITION
///////////////////////////////////////////////////////////////////////////////
typedef enum _ELoggerserviceLogType {
  eLoggerservicelogTypeNormal,
  eLoggerservicelogTypeAbnormal,
  eLoggerservicelogTypeGrpRelaunch,
  eLoggerservicelogTypeOther,
} ELOGGERSERVICELOGTYPE;

class CLoggerUtil {
 public:
  CLoggerUtil(void);

  ~CLoggerUtil(void);

  EFrameworkunifiedStatus Initialize(CLoggerCfg *f_pLoggerCfg);

  EFrameworkunifiedStatus checkDestinationAvailable(TLoggerErrorEvent &f_eventNtfData);  // NOLINT (runtime/references)
  std::string getDestination(TLoggerErrorEvent &f_eventNtfData);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus getFilePathAndName(HANDLE f_hApp,
                                TLoggerErrorEvent &f_eventNtfData,  // NOLINT (runtime/references)
                                uint32_t f_time, std::string &f_pathAndName);  // NOLINT (runtime/references)
  void SetMilage(UI_32 f_milage);

  EFrameworkunifiedStatus getEmmcNaviLogParams(uint32_t f_time,
                                  EPWR_SHUTDOWN_TRIGGER_TYPE errorType,
                                  std::string &f_pathAndName, UI_32 &f_logMax);  // NOLINT (runtime/references)

  EFrameworkunifiedStatus deleteOldLog(std::string log_path,
                          std::vector<std::string> &l_vector,  // NOLINT (runtime/references)
                          std::string &f_archive_destination, UI_32 max_num);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus deleteOldEmmcLog(std::string &f_archive_destination,  // NOLINT (runtime/references)
                              EErrorEventType type);
  static EFrameworkunifiedStatus forceDeleteOldLog(std::string f_archive_destination,
                                      size_t f_req_size);
  static size_t deleteRequestLogs(std::string f_emmcpath, ELOGGERSERVICELOGTYPE f_reqtype,
                                  size_t f_reqsize, ELOGGERSERVICELOGTYPE f_deltype);
  static EFrameworkunifiedStatus CopyFile(std::string f_source, std::string f_destination);
  static EFrameworkunifiedStatus CopyUntyped(std::string f_source,
                                std::string f_destination);
  static EFrameworkunifiedStatus CopyDirectory(std::string f_source,
                                  std::string f_destination);

  static EFrameworkunifiedStatus MoveFile(std::string f_source, std::string f_destination);
  static EFrameworkunifiedStatus MoveUntyped(std::string f_source,
                                std::string f_destination);
  static EFrameworkunifiedStatus MoveDirectory(std::string f_source,
                                  std::string f_destination);

  static ELOGGERSERVICELOGTYPE QueryLoggerservicelogType(std::string f_logname);
  static void SyncDir(std::string f_dir_path);
#ifdef RELEASE_BUILD
  static EFrameworkunifiedStatus PathCheckAndCopyFile(std::string f_source,
      std::string f_destination);
#endif  // RELEASE_BUILD
  void SetUsbMountPath(std::string usbpath);

 private:
  std::string GetUserInvokedFilename(HANDLE f_hApp, uint32_t f_time);
  std::string GetEmmcFilename(uint32_t f_time);
  std::string GetEmmcErrorFilename(uint32_t f_time);
  std::string GetEmmcNaviLogFilename(uint32_t f_time);
  std::string GetTimeString(uint32_t f_time);

  EFrameworkunifiedStatus getNaviLogFilePathAndName(std::string &f_prefix, uint32_t f_time,  // NOLINT (runtime/references)
                                       std::string &f_pathAndName);  // NOLINT (runtime/references)

  CLoggerCfg *m_pCfg;
  UI_32 m_Milage;
  UI_32 m_bootCount;
  static char m_usbpath[USB_PATH_SIZE];
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_UTIL_H_

