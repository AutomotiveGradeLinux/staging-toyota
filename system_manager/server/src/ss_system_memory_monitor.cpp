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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for System Manager System Low Memory
///           detection.
///
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <system_service/ss_sm_thread_names_local.h>
#include <system_service/ss_templates.h>
#include <stub/Clock_API.h>

#include <sys/stat.h>
#include <errno.h>
#include <sys/resource.h>
#include <boost/bind.hpp>
#include <fstream>
#include <sstream>
#include <string>

#include "ss_system_memory_monitor.h"
#include "ss_sm_systemmanagerlog.h"

CSysMemoryMonitor::CSysMemoryMonitor() :
        m_hThread(NULL),
        m_hParentApp(NULL),
        m_SLMCheckCounter(0),
        m_siPriority(-1),
        m_NbrSamplesBeforeSystemmanagerlog(0),
        m_memMonitorThreadName(SS_SMLowMemMonitor),
        m_resmSession(-1) {  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  m_SLMConfig.SLMTimerValue = 0;
  m_SLMConfig.SLMMaxRetryCount = 0;
  m_SLMConfig.SLMThresholdValue = 0;
  m_SLMConfig.SLMSystemmanagerLogIntervalMs = 0;
  bzero(&m_sysMemInfoLast, sizeof(m_sysMemInfoLast));
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

CSysMemoryMonitor::~CSysMemoryMonitor(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  StopAndFreeObjects(m_hParentApp);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus CSysMemoryMonitor::Initialize(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL == f_hApp) {  // LCOV_EXCL_BR_LINE 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusNullPointer;  // LCOV_EXCL_LINE 4:NSFW
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Error. Argument f_hApp passed NULL pointer.");
  } else if (NULL == (m_hThread = FrameworkunifiedCreateChildThreadWithPriority(f_hApp, SS_SMLowMemMonitor, boost::bind(&CSysMemoryMonitor::CPUMemThreadStart, this, _1), boost::bind(&CSysMemoryMonitor::CPUMemThreadStop, this, _1), PR_SMLOWMEMMON))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusInvldHandle;  // LCOV_EXCL_LINE 4:NSFW
    LOG_ERROR("FrameworkunifiedCreateChildThreadWithPriority()");  // LCOV_EXCL_LINE 4:NSFW
  } else if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedStartChildThread(f_hApp, m_hThread, 0, NULL))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("FrameworkunifiedStartChildThread()");  // LCOV_EXCL_LINE 4:NSFW
  } else {
    m_hParentApp = f_hApp;
    m_sysMemInfoLast.TotalMemoryBytes = GetSystemRamSize();
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Info. SLM thread successfully initialized.");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::StopAndFreeObjects(HANDLE f_hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  CALL_AND_LOG_STATUS(FrameworkunifiedStopChildThread(f_hApp, m_hThread, 0, NULL));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  CALL_AND_LOG_STATUS(FrameworkunifiedDestroyChildThread(f_hApp, m_hThread));  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

const std::string CSysMemoryMonitor::GetThreadName(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (m_memMonitorThreadName);
}

void CSysMemoryMonitor::SetSLMConfigData(SLMConfigParameters & f_ConfigData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  m_SLMConfig.SLMTimerValue = f_ConfigData.SLMTimerValue;
  m_SLMConfig.SLMMaxRetryCount = f_ConfigData.SLMMaxRetryCount;
  m_SLMConfig.SLMThresholdValue = f_ConfigData.SLMThresholdValue;
  m_SLMConfig.SLMSystemmanagerLogIntervalMs = f_ConfigData.SLMSystemmanagerLogIntervalMs;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
          "Timer value '%d', Retry Count '%d', threshold '%d', log interval '%dms'",
          m_SLMConfig.SLMTimerValue, m_SLMConfig.SLMMaxRetryCount,
          m_SLMConfig.SLMThresholdValue, m_SLMConfig.SLMSystemmanagerLogIntervalMs);

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EFrameworkunifiedStatus CSysMemoryMonitor::CPUMemThreadStart(HANDLE f_hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  l_eStatus = FrameworkunifiedSubscribeNotificationWithCallback(f_hThread,
          NTFY_ResourceMgr_Availability,
          boost::bind(&CSysMemoryMonitor::OnAvailability, this, _1));
  if (l_eStatus != eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4:NSFW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    SS_ASERT(0);  // LCOV_EXCL_LINE 4:NSFW
  }

  if (FALSE == (m_memMonitorTimer.Initialize(f_hThread, SS_SLM_MEM_MONITOR_TIMER_ID, boost::bind(&CSysMemoryMonitor::OnMemoryTimerExpiry, this, _1)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusBadConnection;  // LCOV_EXCL_LINE 4:NSFW
    LOG_ERROR("m_memMonitorTimer.Initialize() returned 'FALSE'");  // LCOV_EXCL_LINE 4:NSFW
  } else if (FALSE == (m_systemmanagerlogTimer.Initialize(f_hThread, SS_SLM_SYSTEMMANAGERLOG_TIMER_ID, boost::bind(&CSysMemoryMonitor::OnSystemmanagerlogTimerExpiry, this, _1)))) {  // LCOV_EXCL_BR_LINE 4:NSFW  // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    l_eStatus = eFrameworkunifiedStatusBadConnection;  // LCOV_EXCL_LINE 4:NSFW
    LOG_ERROR("m_systemmanagerlogTimer.Initialize() returned 'FALSE'");  // LCOV_EXCL_LINE 4:NSFW
  } else {
    if (m_SLMConfig.SLMTimerValue > 0) {
      m_memMonitorTimer.SetTime(m_SLMConfig.SLMTimerValue / 1000,
              m_SLMConfig.SLMTimerValue % 1000,
              m_SLMConfig.SLMTimerValue / 1000,
              m_SLMConfig.SLMTimerValue % 1000);
    } else {
      // default timer settings
      m_memMonitorTimer.SetTime(SS_MEMORY_TIMER_CONFIG_MS / 1000,
              SS_MEMORY_TIMER_CONFIG_MS % 1000,
              SS_MEMORY_TIMER_CONFIG_MS / 1000,
              SS_MEMORY_TIMER_CONFIG_MS % 1000);

      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
              " Warning. SLM monitor timer configuration invalid. Using default values.");
    }

    if (m_SLMConfig.SLMSystemmanagerLogIntervalMs > 0) {
      m_systemmanagerlogTimer.SetTime(m_SLMConfig.SLMSystemmanagerLogIntervalMs / 1000,
              m_SLMConfig.SLMSystemmanagerLogIntervalMs % 1000,
              m_SLMConfig.SLMSystemmanagerLogIntervalMs / 1000,
              m_SLMConfig.SLMSystemmanagerLogIntervalMs % 1000);
    } else {
      // default timer settings
      m_systemmanagerlogTimer.SetTime(SS_SYSTEMMANAGERLOG_TIMER_CONFIG_MS / 1000,
              SS_SYSTEMMANAGERLOG_TIMER_CONFIG_MS % 1000,
              SS_SYSTEMMANAGERLOG_TIMER_CONFIG_MS / 1000,
              SS_SYSTEMMANAGERLOG_TIMER_CONFIG_MS % 1000);

      FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
              " Warning. SLM FRAMEWORKUNIFIEDLOG timer configuration invalid. Using default values.");
    }

    if (FALSE == (m_memMonitorTimer.Start())) {  // LCOV_EXCL_BR_LINE 4:NSFW
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFault;
      LOG_ERROR("m_memMonitorTimer.Start() returned 'FALSE'");
      // LCOV_EXCL_STOP 4:NSFW
    } else if (FALSE == (m_systemmanagerlogTimer.Start())) {  // LCOV_EXCL_BR_LINE 4:NSFW
      // LCOV_EXCL_START 4:NSFW
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusFault;
      LOG_ERROR("m_systemmanagerlogTimer.Start() returned 'FALSE'");
      // LCOV_EXCL_STOP 4:NSFW
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Success.");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::CPUMemThreadStop(HANDLE f_hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  l_eStatus = StopTimers();
  LOG_STATUS_IF_ERRORED(l_eStatus, "StopTimers");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  if ((uint32_t) -1 != m_resmSession) {
    if (RESM_E_OK != RESM_Close(m_resmSession)) {
      l_eStatus = eFrameworkunifiedStatusFault;
      LOG_ERROR("RESM_Close()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::StopTimers(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (FALSE == (m_memMonitorTimer.Stop())) {  // LCOV_EXCL_BR_LINE 4:NSFW's error
    l_eStatus = eFrameworkunifiedStatusFault;
    LOG_ERROR("m_memMonitorTimer.Stop() returned 'FALSE'");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }

  if (FALSE == (m_systemmanagerlogTimer.Stop())) {  // LCOV_EXCL_BR_LINE 4:NSFW's error
    l_eStatus = eFrameworkunifiedStatusFault;
    LOG_ERROR("m_systemmanagerlogTimer.Stop() returned 'FALSE'");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::OnMemoryTimerExpiry(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  RESM_STATUS_t l_resmStatus;

  if ((uint32_t) -1 == m_resmSession) {
    return eFrameworkunifiedStatusOK;
  }

  if (RESM_E_OK != RESM_GetStatus(m_resmSession, &l_resmStatus)) {
    LOG_ERROR("RESM_GetStatus()");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
    l_eStatus = eFrameworkunifiedStatusFail;
  } else {
    m_sysMemInfoLast.FreeMemoryBytes = l_resmStatus.restMemSize;
    if ((m_sysMemInfoLast.FreeMemoryBytes * 1024)
            < (UI_32) m_SLMConfig.SLMThresholdValue) {
      m_SLMCheckCounter++;

      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, " Low Memory detected(%d/%d).",
              m_SLMCheckCounter, m_SLMConfig.SLMMaxRetryCount);

      if (m_SLMCheckCounter >= m_SLMConfig.SLMMaxRetryCount) {
        l_eStatus = FrameworkunifiedSendParent(hThread,
                eSysMemThrdCmd_SYS_LOW_MEMORY, sizeof(m_sysMemInfoLast),
                &m_sysMemInfoLast);
        LOG_STATUS_IF_ERRORED(l_eStatus,  // LCOV_EXCL_BR_LINE 15: marco defined in ns_logger_if.h  // NOLINT(whitespace/line_length)
                "FrameworkunifiedSendParent(eSysMemThrdCmd_SYS_LOW_MEMORY)");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)

        l_eStatus = StopTimers();
        LOG_STATUS_IF_ERRORED(l_eStatus, "StopTimers");  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
    } else {
      m_SLMCheckCounter = 0;
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::OnSystemmanagerlogTimerExpiry(HANDLE hThread) {
  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_freeMemoryObfuscated;
  UI_32 l_totalMemoryObfuscated;

  l_freeMemoryObfuscated = BitReverse32(m_sysMemInfoLast.FreeMemoryBytes);
  l_totalMemoryObfuscated = BitReverse32(m_sysMemInfoLast.TotalMemoryBytes);

  // Print to ZONE_WARN as per IAT requirement see WI 219056.
  FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "FM: [%d/%d].", l_freeMemoryObfuscated,
          l_totalMemoryObfuscated);

  static int count = 0;

  if (!(count %= 2)) {
    CHAR l_format[256];
    uint32_t timebuf;
    uint8_t status;
    struct tm gmt;
    struct tm local;
    Clock_getSystemTimeY2K38(&timebuf, &status);
    Clock_CnvSecToDateY2K38(&timebuf, &gmt);
    Clock_getLocalTimeY2K38(&timebuf, &local);

    sprintf(l_format, " G%02d %02d%02d%02d L%02d %02d%02d%02d", gmt.tm_mday, // NOLINT
            gmt.tm_hour, gmt.tm_min, gmt.tm_sec, local.tm_mday,
            local.tm_hour, local.tm_min, local.tm_sec);

    FRAMEWORKUNIFIEDLOG(ZONE_STATE, __FUNCTION__, "%s", l_format);
  }

  count++;

  FRAMEWORKUNIFIEDLOG(ZONE_PERIODIC_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

EFrameworkunifiedStatus CSysMemoryMonitor::OnAvailability(HANDLE hThread) {
  if (FrameworkunifiedIsServiceAvailable(hThread)) {
    if (m_resmSession == (uint32_t) -1) {
      if (RESM_E_OK != RESM_Open(NULL, &m_resmSession)) {
          m_resmSession = -1;
          SS_ASERT(0);  // LCOV_EXCL_BR_LINE 15: marco defined in ss_templates.h  // NOLINT(whitespace/line_length)
      }
    }
  }
  return eFrameworkunifiedStatusOK;
}

UI_32 CSysMemoryMonitor::GetSystemRamSize(void) {
  UI_32 l_ramTotalBytes = 0;
  std::ifstream l_sFile("/proc/meminfo");

  if (l_sFile.fail()) {  // LCOV_EXCL_BR_LINE 5: Standard lib error
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Open /proc/meminfo");
  } else {
    ssize_t l_position;
    std::string l_str;

    while (getline(l_sFile, l_str)) {

//      if (std::string::npos
//            != (l_position = static_cast<unsigned int>(l_str.find("MemTotal:", 0)))) {
      if (0 <= (l_position = l_str.find("MemTotal:", 0))) {  // LCOV_EXCL_BR_LINE 200:linux system information

        l_str.erase(l_position, l_position + strlen("MemTotal:"));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        if (std::string::npos != (l_position = static_cast<unsigned int>(l_str.find("kB", 0)))) {  // LCOV_EXCL_BR_LINE 200:linux system information
          l_str.erase(l_position, l_position + strlen("kB"));  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)
        }

        std::istringstream l_istr(l_str);  // LCOV_EXCL_BR_LINE 11:unexpected branch  // NOLINT(whitespace/line_length)

        l_istr >> l_ramTotalBytes;

        break;
      }
    }
  }

  return (l_ramTotalBytes);
}

UI_32 CSysMemoryMonitor::BitReverse32(UI_32 f_val) {
  // Don't print FRAMEWORKUNIFIEDLOG(ZONE_FUNC) to prevent obfuscation method from being revealed.
  f_val = (((f_val & 0xaaaaaaaa) >> 1) | ((f_val & 0x55555555) << 1));
  f_val = (((f_val & 0xcccccccc) >> 2) | ((f_val & 0x33333333) << 2));
  f_val = (((f_val & 0xf0f0f0f0) >> 4) | ((f_val & 0x0f0f0f0f) << 4));
  f_val = (((f_val & 0xff00ff00) >> 8) | ((f_val & 0x00ff00ff) << 8));
  f_val = (f_val >> 16) | (f_val << 16);
  return (f_val);
}
