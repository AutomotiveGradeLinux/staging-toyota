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
/// \brief    Provide support for CPU high load monitor configuration.
///
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// INCLUDES
///////////////////////////////////////////////////////////////////////////////
#include "ss_logger_cfg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <boost/bind.hpp>
#include <native_service/nslogutil_cmd_if.h>
#include <native_service/ns_np_service_protocol.h>
#include <system_service/ss_logger_service_notifications.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

const static UI_32 cMAX_EMMC_COUNTER_VALUE = 100000u;  // NOLINT (build/storage_class)
///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS
///////////////////////////////////////////////////////////////////////////////

CLoggerCfg::CLoggerCfg()
  : m_logMaxFileSize(0),
    m_performanceLogMaxFileSize(0),
    m_emmcOutputMax(0),
    m_emmcOutputErrMax(0),
    m_emmcOutputGrpRelaunchMax(0),
    m_emmcOutputNaviLogMax(0),
    m_emmcOutputNaviLogNmlMax(0),
    m_emmcOutputNaviLogErrMax(0),
    m_emmcOutputPath(""),
    m_emmcOutputNaviLogPath(""),
    m_usb0MountPath(""),
    m_usb1MountPath(""),
    m_sdMountPath(""),
    m_eDevType(eDevUSB1),
    m_UserInvokedLoggingNumber(1u),
    m_EmmcCounter(0u),
    m_eLoggerUDPLoggingStatus(eDeactivate),
    m_eLoggerStatus(SS_LOGGER_DEFAULT_VALUE) {
      m_udpEnvFlag = FALSE;
      std::memset(m_logMask, 0, sizeof(m_logMask));
      std::memset(&m_ExternalLogList, 0, sizeof(&m_ExternalLogList));
    }

CLoggerCfg::~CLoggerCfg(void) {  // LCOV_EXCL_START 14:global instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  if (m_ExternalLogList.p_info != NULL) {
    delete[] m_ExternalLogList.p_info;
    m_ExternalLogList.p_info = NULL;
  }
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CLoggerCfg::Initialize(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  char* l_udp_env = getenv("UDPLOGS_ON");
  m_udpEnvFlag = (
      l_udp_env == NULL ?
          FALSE : (strcmp("TRUE", l_udp_env) == 0 ? TRUE : FALSE));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
EFrameworkunifiedStatus CLoggerCfg::SetandPersistLoggingParams(HANDLE f_hApp,
                                                  STLoggerSetAllParams f_Params,
                                                  BOOL f_InitCall) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  BOOL l_paramChanged = f_InitCall;
  BOOL l_updateLoggerState = f_InitCall;

  if ((TRUE == l_paramChanged)  // LCOV_EXCL_BR_LINE 200:l_paramChanged is never TRUE
      ||
      (m_eLoggerStatus != f_Params.Logger_State)
      ||  // LCOV_EXCL_BR_LINE 200:Because m_UserInvokedLoggingNumber != f_Params.Log_FolderCnt is never false
      (m_UserInvokedLoggingNumber != f_Params.Log_FolderCnt)
      || (m_eLoggerUDPLoggingStatus != f_Params.Logger_UDPState)
      || (m_eDevType != f_Params.Device_Type)) {
    l_paramChanged = TRUE;
  } else {
    l_paramChanged = FALSE;
  }

  l_updateLoggerState =
      ((l_updateLoggerState == TRUE)
          || (m_eLoggerStatus != f_Params.Logger_State)) ? TRUE : FALSE;

  // After startup Logging will be set to default value
  if (TRUE == f_InitCall) {  // LCOV_EXCL_BR_LINE 200:f_InitCall is never TRUE
    // LCOV_EXCL_START 200:f_InitCall is never TRUE
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    m_eLoggerStatus = SS_LOGGER_DEFAULT_VALUE;
    // LCOV_EXCL_STOP
  } else {
    m_eLoggerStatus = f_Params.Logger_State;
  }
  m_eLoggerUDPLoggingStatus = f_Params.Logger_UDPState;
  m_UserInvokedLoggingNumber = f_Params.Log_FolderCnt;
  m_eDevType = f_Params.Device_Type;

  if (TRUE == l_paramChanged) {
    l_eStatus = PersistLoggingParam(f_hApp);
    LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: macro
                          "PersistLoggingParam()");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::SetLoggingParams(HANDLE f_hApp,
                                        const STLoggerSetParams f_stLoggerSetParams) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  STLoggerSetAllParams l_Params;

  l_Params.Logger_UDPState = m_eLoggerUDPLoggingStatus;
  l_Params.Log_FolderCnt = m_UserInvokedLoggingNumber;
  l_Params.Logger_State = f_stLoggerSetParams.Logger_State;
  l_Params.Device_Type = f_stLoggerSetParams.Device_Type;

  l_eStatus = SetandPersistLoggingParams(f_hApp, l_Params, FALSE);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SetandPersistLoggingParams()");  // LCOV_EXCL_BR_LINE 15: macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::SetUDPLogging(HANDLE f_hApp,
                                     const ELOGGER_STAT f_eUDPLoggingStatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  STLoggerSetAllParams l_Params;

  l_Params.Logger_UDPState = f_eUDPLoggingStatus;
  l_Params.Log_FolderCnt = m_UserInvokedLoggingNumber;
  l_Params.Logger_State = m_eLoggerStatus;
  l_Params.Device_Type = m_eDevType;

  l_eStatus = SetandPersistLoggingParams(f_hApp, l_Params, FALSE);
  LOG_STATUS_IF_ERRORED(l_eStatus, "SetandPersistLoggingParams()");  // LCOV_EXCL_BR_LINE 15: macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::PersistLoggingParam(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(
      ZONE_INFO, __FUNCTION__,
      "LoggerState = %d, Device_Type = %d,UDPLogging = %d, TriggerNumber = %d",
      m_eLoggerStatus, m_eDevType, m_eLoggerUDPLoggingStatus,
      m_UserInvokedLoggingNumber);  // LCOV_EXCL_BR_LINE 11:Unexpected branch

  this->UpdateLoggingStatus();
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}

void CLoggerCfg::RemoveCarriageReturn(std::string &f_str) {
  if (f_str.empty()) {  // LCOV_EXCL_BR_LINE 8: Because there is no empty string case
    return;
  }

  if ('\r' == f_str.at(f_str.size() - 1)) {
    f_str.erase(f_str.size() - 1, 1);
  }
}
EFrameworkunifiedStatus CLoggerCfg::Load(const std::string &f_filePathAndName) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusDbRecNotFound;

  if ("" == f_filePathAndName) {  // LCOV_EXCL_BR_LINE 200: Because there is no empty string case
    // LCOV_EXCL_START 200: Because there is no empty string case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Invalid configuration file name detected.");
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    // LCOV_EXCL_STOP
  } else {
    // FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Reading Configuration Data from %s", f_filePathAndName.c_str());

    CNSConfigReader *l_pReaderCfg = new CNSConfigReader();  // LCOV_EXCL_BR_LINE 11:except,C++ STL
    if (NULL == l_pReaderCfg) {  // LCOV_EXCL_BR_LINE 5:new error
      // LCOV_EXCL_START 5:new error
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error. Unable to allocate CNSConfigReader object.");
      l_eStatus = eFrameworkunifiedStatusFileLoadError;
      // LCOV_EXCL_STOP
    } else {
      if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->Parse(f_filePathAndName))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error. l_pReaderCfg->Parse(%s) returned: 0x%X",
               f_filePathAndName.c_str(), l_eStatus);
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.LogMaxFileSize", m_logMaxFileSize))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetInt(LOGGING.LogMaxFileSize)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.PerformanceLogSize", m_performanceLogMaxFileSize))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetInt(LOGGING.LogMaxFileSize)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("LOGGING.EmmcOutputPath", m_emmcOutputPath))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.EmmcOutputPath)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("LOGGING.EmmcOutputNaviLogPath", m_emmcOutputNaviLogPath))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputNaviLogPath)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("LOGGING.Usb0MountPath", m_usb0MountPath))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.Usb0MountPath)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("LOGGING.Usb1MountPath", m_usb1MountPath))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.Usb1MountPath)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("LOGGING.SdMountPath", m_sdMountPath))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.SdMountPath)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputMax", m_emmcOutputMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.EmmcOutputMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputErrMax", m_emmcOutputErrMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputErrMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputGrpRelaunchMax", m_emmcOutputGrpRelaunchMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputGrpRelaunchMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputNaviLogMax", m_emmcOutputNaviLogMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputNaviLogMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputNaviLogNmlMax", m_emmcOutputNaviLogNmlMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputNaviLogNmlMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.EmmcOutputNaviLogErrMax", m_emmcOutputNaviLogErrMax))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus,
                   "l_pReaderCfg->GetString(LOGGING.EmmcOutputNaviLogErrMax)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetString("STORAGE.ThreadName", m_loggerStorageInfo.Name))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(STORAGE.ThreadName)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("STORAGE.Priority", m_loggerStorageInfo.Priority))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetInt(STORAGE.Priority)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("STORAGE.Timeout", m_loggerStorageInfo.Timeout))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetInt(STORAGE.Timeout)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("STORAGE.TimeoutAfter", m_loggerStorageInfo.TimeoutAfter))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        // LCOV_EXCL_START 200:To ensure success
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetInt(STORAGE.TimeoutAfter)");
        // LCOV_EXCL_STOP
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.ExternalLogNum", m_ExternalLogList.num))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.ExternalLogNum)");  // LCOV_EXCL_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
      } else if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt("LOGGING.ExternalLogMax", m_ExternalLogList.max))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
        LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(LOGGING.ExternalLogMax)");  // LCOV_EXCL_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
      } else {
        RemoveCarriageReturn(m_emmcOutputPath);
        RemoveCarriageReturn(m_emmcOutputNaviLogPath);
        RemoveCarriageReturn(m_usb0MountPath);
        RemoveCarriageReturn(m_usb1MountPath);
        RemoveCarriageReturn(m_sdMountPath);
        RemoveCarriageReturn(m_loggerStorageInfo.Name);

        for (UI_32 i = 0; i < ZONE_MASK_ARRAY_ELTS; i++) {
          SI_32 l_logMaskTemp;
          std::stringstream l_cfgParamName;

          l_cfgParamName << "SETLOGMASK.MASK" << i;
          if (eFrameworkunifiedStatusOK != (l_eStatus = l_pReaderCfg->GetInt(l_cfgParamName.str(), l_logMaskTemp))) {  // LCOV_EXCL_BR_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
            // LCOV_EXCL_START 200:To ensure success
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                   " Error. l_pReaderCfg->GetInt(%s) failed with error: %d.",
                   l_cfgParamName.str().c_str(), l_eStatus);

            break;
            // LCOV_EXCL_STOP
          } else {
            m_logMask[i] = (UI_32) l_logMaskTemp;
          }
        }

        m_EELStorageRelPath = "/EEL_export";
        m_EELActivateRelPath = "/EEL_activate";
        m_LoggerserviceEmmcLogsRelPath = "/LOGGERSERVICE_EMMC_LOGS";

        if (m_ExternalLogList.num > 0) {
          std::string str = "EXTERNAL_";  // LCOV_EXCL_BR_LINE 11:Excluded due to gcov constraints (others)
          UI_32 num = 0;

          if (m_ExternalLogList.num <= m_ExternalLogList.max) {
            num = m_ExternalLogList.num;
          } else {
            num = m_ExternalLogList.max;
          }

          // LCOV_EXCL_BR_START 11:Excluded due to gcov constraints (others)
          m_ExternalLogList.p_info = new ExternalLogInfo[num];

          // Load external log info
          for (UI_32 i = 0; i < num; i++) {
            std::string query = str + std::to_string(i+1);
            if (eFrameworkunifiedStatusOK !=
                (l_eStatus = l_pReaderCfg->GetString((query+".OutputPath"), m_ExternalLogList.p_info[i].dirname))) {
              LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(EXTERNAL.OutputPath)");  // LCOV_EXCL_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
            } else if (eFrameworkunifiedStatusOK !=
                (l_eStatus = l_pReaderCfg->GetString((query+".ExtLogPath"), m_ExternalLogList.p_info[i].path))) {
              LOG_STATUS(l_eStatus, "l_pReaderCfg->GetString(EXTERNAL.ExtLogPath)");  // LCOV_EXCL_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
            } else if (eFrameworkunifiedStatusOK !=
                (l_eStatus = l_pReaderCfg->GetBool((query+".Remove"), m_ExternalLogList.p_info[i].remove))) {
              LOG_STATUS(l_eStatus, "l_pReaderCfg->GetBool(EXTERNAL.Remove)");  // LCOV_EXCL_LINE 200:To ensure success  // NOLINT[whitespace/line_length]
            } else {
              RemoveCarriageReturn(m_ExternalLogList.p_info[i].dirname);
              RemoveCarriageReturn(m_ExternalLogList.p_info[i].path);
            }
          }
          // LCOV_EXCL_BR_STOP 11:Excluded due to gcov constraints (others)
        } else {
          // None external log
        }

        if (eFrameworkunifiedStatusOK == l_eStatus) {  // LCOV_EXCL_BR_LINE 200:To ensure success
          FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
                 " Info. Configuration successfully loaded.");  // LCOV_EXCL_BR_LINE 15: macro
        }
      }

      delete l_pReaderCfg;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::Validate(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldParam;

  if (m_logMaxFileSize == 0) {  // LCOV_EXCL_BR_LINE 200:Because the size is never 0
    // LCOV_EXCL_START 200:Because the size is never 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected. LogMaxFileSize: %d Must be > 0.",
        m_logMaxFileSize);
    // LCOV_EXCL_STOP
  } else if (m_emmcOutputPath == "") {
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected."
        " EmmcOutputPath: %s Must be a valid path containing a trailing backslash.",
        m_emmcOutputPath.c_str());
  } else if (m_emmcOutputNaviLogPath == "") {  // LCOV_EXCL_BR_LINE 200:Because it is not always an empty string
    // LCOV_EXCL_START 200:Because it is not always an empty string
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected."
        " EmmcOutputNaviLogPath: %s Must be a valid path containing a trailing backslash.",
        m_emmcOutputNaviLogPath.c_str());
    // LCOV_EXCL_STOP
  } else if (m_usb0MountPath == "") {  // LCOV_EXCL_BR_LINE 200:Because it is not always an empty string
    // LCOV_EXCL_START 200:Because it is not always an empty string
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected."
        " Usb0MountPath: %s Must be a valid path containing a trailing backslash.",
        m_usb0MountPath.c_str());
    // LCOV_EXCL_STOP
  } else if (m_usb1MountPath == "") {  // LCOV_EXCL_BR_LINE 200:Because it is not always an empty string
    // LCOV_EXCL_START 200:Because it is not always an empty string
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected."
        " Usb1MountPath: %s Must be a valid path containing a trailing backslash.",
        m_usb1MountPath.c_str());
    // LCOV_EXCL_STOP
  } else if (m_sdMountPath == "") {  // LCOV_EXCL_BR_LINE 200:Because it is not always an empty string
    // LCOV_EXCL_START 200:Because it is not always an empty string
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(
        ZONE_ERR,
        __FUNCTION__,
        " Error. Invalid configuration detected. SdMountPath: %s Must be a valid path containing a trailing backslash.",
        m_sdMountPath.c_str());
    // LCOV_EXCL_STOP
  } else {
    l_eStatus = eFrameworkunifiedStatusOK;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

void CLoggerCfg::Print(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

std::string CLoggerCfg::getEmmcOutputPath(void) {
  return (m_emmcOutputPath);
}

std::string CLoggerCfg::getEmmcOutputNaviLogPath(void) {
  return (m_emmcOutputNaviLogPath);
}

std::string CLoggerCfg::getUsb0MountPath(void) {
  return (m_usb0MountPath);
}
void CLoggerCfg::setUsb0MountPath(std::string& f_deviceMountpath) {
  m_usb0MountPath = f_deviceMountpath;
}

EFrameworkunifiedStatus CLoggerCfg::GetUserInvokedDestination(std::string& f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  f_dest = "";
  switch (m_eDevType) {
    case eDevUSB1:
      f_dest = m_usb0MountPath;
      break;

    case eDevUSB2:
      f_dest = m_usb1MountPath;
      break;

    case eDevSD:
      f_dest = m_sdMountPath;
      break;

    case eTotalDevicesTypes:
    case eInvalid_DevType:
    default:
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Unknown device type: %d",
             m_eDevType);
      l_eStatus = eFrameworkunifiedStatusFail;
      break;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::GetEmmcDestination(std::string& f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  f_dest = m_emmcOutputPath;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::GetEmmcNaviLogDestination(std::string& f_dest) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  f_dest = m_emmcOutputNaviLogPath;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

UI_32 CLoggerCfg::GetUserInvokedCounter(void) {
  return m_UserInvokedLoggingNumber;
}

EFrameworkunifiedStatus CLoggerCfg::GetIncrementAndPersistUserInvokedCounter(
    HANDLE f_hApp, UI_32 &f_counter) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  f_counter = m_UserInvokedLoggingNumber;
  m_UserInvokedLoggingNumber++;
  l_eStatus = PersistLoggingParam(f_hApp);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::GetAndIncrementEmmcCounter(UI_32 &f_counter) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_checkFileName;
  std::vector<UI_32> l_vector;
  DIR *l_dirp;
  struct dirent l_dirent;
  struct dirent* next = NULL;
  size_t l_pos;
  UI_32 l_count;
  UI_32 l_maxCount;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  if (m_EmmcCounter == 0u) {
    l_maxCount = 0;
    l_dirp = opendir(m_emmcOutputPath.c_str());
    if (l_dirp != NULL) {  // LCOV_EXCL_BR_LINE 5: m_emmcOutputPath is not null, opendir is C code, always ok
      for (;;) {
        int l_ret = readdir_r(l_dirp, &l_dirent, &next);
        if (((l_ret == 0) && (next == NULL))
            || (l_ret > 0)) {
          break;
        }
        l_checkFileName = l_dirent.d_name;
        l_pos = l_checkFileName.find(".tar.gz");
        if (std::string::npos != l_pos) {
          l_pos = l_checkFileName.find("_");
          if (std::string::npos != l_pos) {  // LCOV_EXCL_BR_LINE 200: tar.gz file is aways has "_"
            std::string l_numberString = l_checkFileName.substr(0, l_pos);
            l_count = static_cast<UI_32>(strtoul(l_numberString.c_str(), NULL, 10));
            l_vector.push_back(l_count);
            l_maxCount = std::max(l_count, l_maxCount);
          }
        }
      }
      closedir(l_dirp);
    } else {
      // LCOV_EXCL_START 5: m_emmcOutputPath is not null, opendir is C code, always ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      f_counter = 1;
      return eFrameworkunifiedStatusOK;
      // LCOV_EXCL_STOP
    }

    // reach at max number
    if (l_maxCount >= LOGGERSERVICELOG_EMMC_COUNTER_MAX) {
      RefreshEmmcCounter(m_EmmcCounter);
    } else {
      m_EmmcCounter = l_maxCount + 1;
    }
  } else if (m_EmmcCounter >= LOGGERSERVICELOG_EMMC_COUNTER_MAX) {
    // reach at max number
    RefreshEmmcCounter(m_EmmcCounter);
  } else {
    m_EmmcCounter++;
  }

  f_counter = m_EmmcCounter;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CLoggerCfg::RefreshEmmcCounter(UI_32& f_counter) const {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  std::string l_checkFileName;
  std::vector<std::string> l_vector;
  DIR *l_dirp;
  struct dirent l_dirent;
  struct dirent* next = NULL;
  size_t l_pos;

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // get filename saved frameworkunifiedlog on eMMC Device
  l_dirp = opendir(m_emmcOutputPath.c_str());
  if (l_dirp != NULL) {  // LCOV_EXCL_BR_LINE 5: m_emmcOutputPath is not null, opendir is C code, always ok
    for (;;) {
      int l_ret = readdir_r(l_dirp, &l_dirent, &next);
      if (((l_ret == 0) && (next == NULL)) || (l_ret > 0)) {
        break;
      }
      l_checkFileName = l_dirent.d_name;
      l_pos = l_checkFileName.find(".tar.gz");
      if (std::string::npos != l_pos) {
        l_pos = l_checkFileName.find("_");
        if (std::string::npos != l_pos) {
          l_vector.push_back(l_checkFileName);
        }
      }
    }
    closedir(l_dirp);
  } else {
    // LCOV_EXCL_START 5: m_emmcOutputPath is not null, opendir is C code, always ok
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    // ignore opendir failure. this means saved frameworkunifiedlog is nothing on eMMC Device
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open dir error path=%s errno=%s.",
           m_emmcOutputPath.c_str(), strerror(errno));
    f_counter = 1;
    return eFrameworkunifiedStatusOK;
    // LCOV_EXCL_STOP
  }

  // sort at log count
  std::sort(l_vector.begin(), l_vector.end());

  std::vector<std::string>::iterator it;
  std::string l_orgFilePath;
  std::string l_renameFilePath;
  UI_32 l_wpt = 0;
  UI_32 l_count;

  it = l_vector.begin();
  while (it != l_vector.end()) {
    std::stringstream l_nameStream;
    // get log counter parts
    l_wpt++;
    l_checkFileName = *it;
    l_pos = l_checkFileName.find("_");
    std::string l_numberString = l_checkFileName.substr(0, l_pos);
    l_count = static_cast<UI_32>(strtoul(l_numberString.c_str(), NULL, 10));

    // rename file if necessary
    if (l_count != l_wpt) {
      l_renameFilePath = l_orgFilePath = m_emmcOutputPath.c_str();
      l_renameFilePath.append("/");
      l_orgFilePath.append("/");
      l_orgFilePath.append(l_checkFileName);

      l_nameStream << std::setw(5) << std::setfill('0') << std::right
                   << std::dec << l_wpt;
      l_checkFileName.replace(0, l_pos, l_nameStream.str());

      l_renameFilePath.append(l_checkFileName);

      if (rename(l_orgFilePath.c_str(), l_renameFilePath.c_str()) != 0) {  // LCOV_EXCL_BR_LINE 5: rename is C code
        // LCOV_EXCL_START 5: rename is C code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // rename failed but continue reamin log rename because new log need to be saved.
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "log file[%s]->[%s] rename failed: err=%s.",
               l_orgFilePath.c_str(), l_renameFilePath.c_str(), strerror(errno));
        // LCOV_EXCL_STOP
      }
    }
    it++;
  }

  f_counter = l_wpt + 1;

  // set return value as next log counter
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

BOOL CLoggerCfg::IsUDPLoggingEnabled(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_ret =
      ((this->m_eLoggerUDPLoggingStatus == eActivate)
          || (this->m_udpEnvFlag == TRUE)) ? TRUE : FALSE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "- : %s", (TRUE == l_ret)?"TRUE":"FALSE");
  return (l_ret);
}

void CLoggerCfg::AddLoggingParamCB(CbFuncPtr f_fp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  if (f_fp != NULL) {  // LCOV_EXCL_BR_LINE 6: it aways not be null
    this->m_LoggingCallBack.push_back(f_fp);
    (void) f_fp(NULL);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

void CLoggerCfg::UpdateLoggingStatus(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  std::vector<CbFuncPtr>::iterator l_iter;
  for (l_iter = m_LoggingCallBack.begin(); l_iter != m_LoggingCallBack.end(); l_iter++) {  // LCOV_EXCL_BR_LINE 11:Unexpected branch  // NOLINT[whitespace/line_length]
    (void) (*l_iter)(NULL);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus CLoggerCfg::GetLoggerStorageInfo(SLoggerStorageInfo &f_info) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  f_info = m_loggerStorageInfo;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

BOOL CLoggerCfg::IsLoggingEnabled(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  return (this->m_eLoggerStatus == eActivate) ? TRUE : FALSE;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

ExternalLogList CLoggerCfg::GetExternalLogList(void) {
  return (m_ExternalLogList);
}  // LCOV_EXCL_BR_LINE 10:the last line
