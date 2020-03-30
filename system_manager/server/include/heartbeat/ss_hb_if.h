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
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for the application heartbeat system.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef HEARTBEAT_INTERFACE_H_  // NOLINT
#define HEARTBEAT_INTERFACE_H_

#include <system_service/ss_system_manager_if.h>
#include <system_service/ss_system_manager_if_local.h>

/**
 * Enumeration: message category
 */
typedef enum HB_STATUS {
  HB_STATUS_INVALID
  , HB_STATUS_REGISTERED
  , HB_STATUS_GOOD
  , HB_STATUS_FAILED
  , HB_STATUS_TIMEOUT
} EHBStatus;

/*
 * Structure: report message data
 */
typedef struct MODULE_INFO {
#define SS_SM_HB_MAX_PROC_NAME_SIZE 16
  EHBStatus   ProcHBState;
  UI_32       HeartBeatRetryCount;
  // "ProcQueueName" is limited 16 bytes (included terminal-character NULL), so that the size of "THbReportData" will be lower than 4K Bytes.
  CHAR        ProcQueueName[SS_SM_HB_MAX_PROC_NAME_SIZE];
} TSmModuleInfo;

typedef struct HB_REPORT_DATA {
  EHBStatus       eEntireState;   ///< entire state for a request.
                                  ///< if all modules are success, it's success, otherwise failed.
  UI_32           nNumOfModules;  ///< number of modules
  TSmModuleInfo   tModuleList[SS_MAX_NUM_MODULES];    ///< module information list, which include module IDs and states
} THbReportData;

typedef struct HB_AVAIL_CHECK {
  BOOL isOk;
  CHAR serviceName[SS_SM_HB_MAX_PROC_NAME_SIZE];
} THbAvailCheck;


EFrameworkunifiedStatus HBThreadStart(HANDLE hThread);
EFrameworkunifiedStatus HBThreadStop(HANDLE hThread);

#endif  // HEARTBEAT_INTERFACE_H_  // NOLINT
