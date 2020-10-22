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

#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_SUBSYS_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_SUBSYS_H_

#include "system_service/tskm_local_type.h"
#include "tskm_main.h"

typedef struct {
  pid_t pid;
  uint8_t type;  // CL_MonitorType_t
} TSKM_HUNG_INFO_t;

int tskm_sub_init(void);
int tskm_sub_term(void);
void tskm_sub_reboot(const TSKM_ERROR_REBOOT_t* p_info);
void tskm_sub_reboot_normal(void);
void tskm_sub_logging(const TSKM_LOGGING_INFO_t* p_info);
void tskm_sub_debugDumpRes(BOOL bIsNeedSvcName, const char *p_dumpMsg);
int tskm_sub_getExtBootInfo(T_SS_SM_START_ExtDataStructType *p_info);

TSKM_HUNG_INFO_t *tskm_sub_searchHungSvcs();
int tskm_sub_resmInit();
int tskm_sub_resmRcv(TSKM_EVENT_INFO_t* p_ev);
int tskm_sub_setWakeupOrder(const char* p_order);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_SUBSYS_H_

