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
/// \brief    Provide support for SS Logger module configuration.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_CFG_H_
#define LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_CFG_H_

///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_config_parser_if.h>
#include <native_service/ns_logger_if.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_logger_service_local.h>
#include <string>
#include <vector>
#include "loggerservicedebug_loggerservicelog.h"

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION CLASS DEFINITION
///////////////////////////////////////////////////////////////////////////////
/*Default values for the storage thread*/
#define ERROR_EVENT_STORAGE_QUEUE_NAME  "/SSLoggerStrage"
#define ERROR_EVENT_STORAGE_PRIORITY    (UI_8)30
#define ERROR_EVENT_STORAGE_SLEEP_TIME  (useconds_t)50000
#define ERROR_EVENT_STORAGE_SLEEP_AFTER (UI_8)5

#if ENABLE_PRODUCTION_BUILD == 1
#define SS_LOGGER_DEFAULT_VALUE eDeactivate
#else
#define SS_LOGGER_DEFAULT_VALUE eActivate
#endif

#define LOGGERSERVICELOG_EMMC_COUNTER_MAX 99999u

struct SLoggerStorageInfo {
  SLoggerStorageInfo()
      : Name(ERROR_EVENT_STORAGE_QUEUE_NAME),
        Priority(ERROR_EVENT_STORAGE_PRIORITY),
        Timeout(ERROR_EVENT_STORAGE_SLEEP_TIME),
        TimeoutAfter(ERROR_EVENT_STORAGE_SLEEP_AFTER) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }
  std::string Name;
  SI_32 Priority;
  SI_32 Timeout;
  SI_32 TimeoutAfter;
};

enum EInsertEventType {
  eInsertEventEELStorage,
  eInsertEventEELActive,
  eInsertEventLoggerserviceEmmcLogs,
  eInsertEventInvalid
};

class CLoggerCfg {
 public:
  CLoggerCfg(void) noexcept;
  ~CLoggerCfg(void);

  EFrameworkunifiedStatus Initialize(HANDLE f_hApp);

  EFrameworkunifiedStatus Load(const std::string &f_filePathAndName);
  EFrameworkunifiedStatus Validate(void);
  void Print(void);

  EFrameworkunifiedStatus SetLoggingParams(HANDLE f_hApp,
                              STLoggerSetParams f_stLoggerSetParams) noexcept;

  EFrameworkunifiedStatus SetUDPLogging(HANDLE f_hApp, ELOGGER_STAT f_eUDPLoggingStatus)
      noexcept;

  EFrameworkunifiedStatus PersistLoggingParam(HANDLE f_hApp);
  std::string getEmmcOutputPath(void);
  std::string getEmmcOutputNaviLogPath(void);
  std::string getUsb0MountPath(void);
  std::string getUsb1MountPath(void);
  std::string getSdMountPath(void);
  std::string getDeviceName(void);
  void setUsb0MountPath(std::string &f_deviceMountpath);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus GetUserInvokedDestination(std::string &f_dest);  // NOLINT (runtime/references)

  EFrameworkunifiedStatus GetEmmcDestination(std::string &f_dest);  // NOLINT (runtime/references)
  EFrameworkunifiedStatus GetEmmcNaviLogDestination(std::string &f_dest);  // NOLINT (runtime/references)

  UI_32 GetUserInvokedCounter(void);
  EFrameworkunifiedStatus GetIncrementAndPersistUserInvokedCounter(HANDLE f_hApp,
                                                      UI_32 &f_counter);  // NOLINT (runtime/references)

  EFrameworkunifiedStatus GetAndIncrementEmmcCounter(UI_32 &f_counter) noexcept;  // NOLINT (runtime/references)
  EFrameworkunifiedStatus RefreshEmmcCounter(UI_32 &f_counter) noexcept;  // NOLINT (runtime/references)

  void ResetEmmcCounter(void);

  SI_32 m_logMaxFileSize;
  UI_32 m_logMask[ZONE_MASK_ARRAY_ELTS];
  SI_32 m_performanceLogMaxFileSize;
  SI_32 m_emmcOutputMax;
  SI_32 m_emmcOutputErrMax;
  SI_32 m_emmcOutputGrpRelaunchMax;
  SI_32 m_emmcOutputNaviLogMax;
  SI_32 m_emmcOutputNaviLogNmlMax;
  SI_32 m_emmcOutputNaviLogErrMax;

  std::string m_emmcOutputPath;
  std::string m_emmcOutputNaviLogPath;
  std::string m_usb0MountPath;
  std::string m_usb1MountPath;
  std::string m_sdMountPath;

  EDEV_TYPE GetConnectedDevice(std::string f_devPath);
  EInsertEventType GetInsertEventType(std::string f_relPath);

  std::string GetEELActivateRelPath(void);
  std::string GetDebugFolderContentRelPath(void);
  std::string GetEELExportRelPath(void);

  BOOL IsUDPLoggingEnabled(void);
  void AddLoggingParamCB(CbFuncPtr f_fp);

  EFrameworkunifiedStatus GetLoggerStorageInfo(SLoggerStorageInfo &f_info);  // NOLINT (runtime/references)
  BOOL IsLoggingEnabled(void);
  EFrameworkunifiedStatus EelActivateLogging(HANDLE f_hApp);

 private:
  void UpdateLoggingStatus(void) noexcept;
  std::vector<CbFuncPtr> m_LoggingCallBack;
  void GetHighestEmmcCounterValue(UI_32 &f_counter);  // NOLINT (runtime/references)

  EDEV_TYPE m_eDevType;
  UI_32 m_UserInvokedLoggingNumber;
  UI_32 m_EmmcCounter;
  ELOGGER_STAT m_eLoggerUDPLoggingStatus;
  BOOL m_udpEnvFlag;
  ELOGGER_STAT m_eLoggerStatus;

  std::string m_EELStorageRelPath;

  std::string m_EELActivateRelPath;

  std::string m_LoggerserviceEmmcLogsRelPath;

  EFrameworkunifiedStatus SetandPersistLoggingParams(HANDLE f_hApp,
                                        STLoggerSetAllParams f_Params,
                                        BOOL f_InitCall);
  SLoggerStorageInfo m_loggerStorageInfo;

  void RemoveCarriageReturn(std::string &f_str);  // NOLINT (runtime/references)
};

#endif  // LOGGER_SERVICE_SERVER_INCLUDE_SS_LOGGER_CFG_H_

