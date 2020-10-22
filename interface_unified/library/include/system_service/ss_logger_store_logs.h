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

/**
 * @file ss_logger_store_logs.h
 * @brief    This file supports the logger service interface for SystemManager.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_LOGGER_STORE_LOGS_H_  // NOLINT (build/header_guard)
#define SS_LOGGER_STORE_LOGS_H_

#include <native_service/frameworkunified_types.h>
#include "system_service/interfaceunified_system_types.h"

#include <string>
#include <vector>


#ifdef _cplusplus
extern "C" {
#endif
#define SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG "/ramd/BS/ss/logger_service/rwdata/frameworkunifiedlog/SS_LOGGER_SAVE_INTERFACEUNIFIEDLOG_FLAG"

typedef enum {
  SS_STORELOGS_INTERFACEUNIFIEDLOG = 0,
  SS_STORELOGS_ILLEGAL,
  SS_STORELOGS_ACCOFFON,

  SS_STORELOGS_SYS_ILLEGAL,
  SS_STORELOGS_ACCOFFON_PRESS,

  SS_STORELOGS_MAX
} SS_STORELOGS_OPE_TYPE;
///////////////////////////////////////////////////////////////////////////////
/// \ingroup  logger_service
///
/// \brief    SSLoggerSrvIfWriteDebugLogs
///
/// \return Status
///         EFrameworkunifiedStatus - success or error
///////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SS_LoggerStoreLogs(SS_STORELOGS_OPE_TYPE type);

EFrameworkunifiedStatus SS_LoggerStoreLogs_deleteOldLogAbnrm(const std::string& log_path, std::vector<std::string>& l_vector,
                                                const std::string& f_archive_destination, SI_32 max_num,
                                                UI_32 abnrm_total);

EFrameworkunifiedStatus StartRtUsbLogThread(HANDLE hApp);
EFrameworkunifiedStatus StopRtUsbLogThread(HANDLE hApp);

#ifdef _cplusplus
}
#endif

#endif /* SS_LOGGER_STORE_LOGS_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
