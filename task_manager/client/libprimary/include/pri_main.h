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

#ifndef TASK_MANAGER_CLIENT_LIBPRIMARY_INCLUDE_PRI_MAIN_H_
#define TASK_MANAGER_CLIENT_LIBPRIMARY_INCLUDE_PRI_MAIN_H_

#include <native_service/frameworkunified_types.h>

#include "system_service/tskm_type.h"
#include "system_service/Primary_common.h"
#include "system_service/INI_API.hpp"

int pri_main(T_PRIM_PRM* p_prm, int argc, char* argv[]);
void pri_exitStart(void* rsv);
void pri_exitDone(int status);
void* pri_getPrivate();
HANDLE pri_getHandle();
int32_t pri_setMonitorTimeout(uint32_t timeout);

int32_t pri_stepForkComp(uint64_t id);
int32_t pri_accOffComp(uint64_t id);

int32_t pri_getBootInfo(T_SS_SM_START_DataStructType *info);
int32_t pri_getExtBootInfo(T_SS_SM_START_ExtDataStructType *info);
void pri_sendDebugDumpRes(const char *buf);

void pri_init(T_PRIM_PRM* p_prm, int argc, char* argv[], int *fdNum,
              int fdlist[INI_FD_MAX]);
BOOL pri_handler(fd_set* p_fds);
void pri_term(void);
int pri_setMonitorState(BOOL bIsRun, uint32_t timeout);
void pri_getAppHandle(void);

#endif  // TASK_MANAGER_CLIENT_LIBPRIMARY_INCLUDE_PRI_MAIN_H_

