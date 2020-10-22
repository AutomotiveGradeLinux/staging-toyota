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
 * @file ss_power_service_local.h
 * @brief \~english This file supports the Power Service interface.
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
#ifndef SS_POWER_SERVICE_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_POWER_SERVICE_LOCAL_H_


#include <native_service/frameworkunified_types.h>
#include <map>
#include "system_service/ss_system_types.h"





const UI_32  MaxTestCaseName = 128;
const UI_32  MaxRespMsg      = 2048;

typedef enum _PwLVIStatus {
  ePwSrvLVI_Status_InActive,
  ePwSrvLVI_Status_Active
} PowerSrvLVIStatus, EPWER_LVI_STATUS;

typedef struct _PwCompleteAck {
  UI_16 unSessionId;
  CHAR  szServiceName[MAX_QUEUE_NAME_SIZE];
} StartCompleteAck;

typedef struct _SS_PSCurrentState {
  CHAR  testCaseId[MaxTestCaseName];
  CHAR  printRespmsg[MaxRespMsg];
}SS_PSCurrentState;


#endif /* SS_POWER_SERVICE_LOCAL_H_ */  // NOLINT (build/header_guard)
/** @}*/  // end of power_service
/** @}*/  // end of SystemService
/** @}*/  // end of BaseSystem
