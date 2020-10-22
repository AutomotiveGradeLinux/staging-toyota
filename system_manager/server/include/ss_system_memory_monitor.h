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

#ifndef __SS_SYSTEM_MEMORY_MONITOR_H__  // NOLINT
#define __SS_SYSTEM_MEMORY_MONITOR_H__

#include <native_service/frameworkunified_types.h>
#include <system_service/resm.h>
#include <system_service/ss_system_timer.h>
#include <iostream>
#include <string>
#include "ss_sm_config.h"


typedef enum _ESMSysMemThreadCommandIds {
  eSysMemThrdCmd_SYS_LOW_MEMORY = 0x744,  ///< SM.CrashDetector --> SM
} ESMSysMemThreadCommandIds;

struct SysMem {
  UI_32 FreeMemoryBytes;
  UI_32 TotalMemoryBytes;
};

const UI_32 SS_SLM_MEM_MONITOR_TIMER_ID = 3000;
const UI_32 SS_SLM_SYSTEMMANAGERLOG_TIMER_ID      = 3001;
const UI_32 SS_MEMORY_TIMER_CONFIG_MS   = 1000;
const UI_32 SS_SYSTEMMANAGERLOG_TIMER_CONFIG_MS   = 30000;

class CSysMemoryMonitor {
 public:
                     CSysMemoryMonitor(void);

  virtual           ~CSysMemoryMonitor(void);

  EFrameworkunifiedStatus         Initialize(HANDLE f_hApp);
  EFrameworkunifiedStatus         StopAndFreeObjects(HANDLE f_hApp);

  const std::string  GetThreadName(void);
  void               SetSLMConfigData(SLMConfigParameters &f_ConfigData);  // NOLINT

 private:
  EFrameworkunifiedStatus CPUMemThreadStart(HANDLE f_hThread);
  EFrameworkunifiedStatus CPUMemThreadStop(HANDLE f_hThread);
  EFrameworkunifiedStatus StopTimers(void);
  EFrameworkunifiedStatus OnMemoryTimerExpiry(HANDLE f_hThread);
  EFrameworkunifiedStatus OnSystemmanagerlogTimerExpiry(HANDLE f_hThread);
  EFrameworkunifiedStatus OnAvailability(HANDLE f_hThread);

  UI_32      GetSystemRamSize(void);
  UI_32      BitReverse32(UI_32  f_val);

  HANDLE              m_hThread;
  HANDLE              m_hParentApp;
  Timer               m_memMonitorTimer;
  Timer               m_systemmanagerlogTimer;
  SLMConfigParameters m_SLMConfig;
  SI_32               m_SLMCheckCounter;
  SI_32               m_siPriority;
  UI_32               m_NbrSamplesBeforeSystemmanagerlog;
  SysMem              m_sysMemInfoLast;
  std::string         m_memMonitorThreadName;
  uint32_t            m_resmSession;
};

#endif // __SS_SYSTEM_MEMORY_MONITOR_H__  // NOLINT
