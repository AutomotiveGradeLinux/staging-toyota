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

#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_PF_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_PF_H_

#include <native_service/cl_process.h>

#include "system_service/tskm_svc.h"
#include "system_service/tskm_local_type.h"
#include "tskm_main.h"

int tskm_pf_sysInit();
int tskm_pf_procInit();
pid_t tskm_pf_createProc(TSKM_SVC_ATTR_t* p_svcAttr);
int tskm_pf_cleanupProc(int sigFd, pid_t* p_pid, TSKM_ERR_t* p_err);
int tskm_pf_terminateProcGroup(uint16_t pid);
int tskm_pf_shmCreate(const char* name, int32_t size, void** p_addr);
int tskm_pf_shmDelete(const char* name);
int tskm_pf_createThread(void* (*threadFunc)(void*), void* prm,
                         uint32_t priority, const char* threadName,
                         pthread_t* p_thId);
int tskm_pf_sendStopCompResp();
int tskm_pf_abort();
int tskm_pf_exit(int status);
int tskm_pf_touch(char* path);
int tskm_pf_mkTouchFileName(pid_t pid, char name[32]);
int tskm_pf_nvFileInit(HANDLE hApp);
int tskm_pf_nvFileRead(HANDLE hApp, TSKM_NV_INFO_t* p_nvInfo);
int tskm_pf_nvFileWrite(const TSKM_NV_INFO_t* p_nvInfo);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_PORT_PF_H_

