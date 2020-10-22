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

// Communication module header file
#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_COMM_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_COMM_H_

#include <sys/types.h>
#include "system_service/tskm_local_type.h"

#define TSKM_SOCKET_NAME      "/tmp/tskm_sock"  // Sockets for Primary Library

#define TSKM_COMM_CONNECT_MAX   (32)

typedef struct {
  pid_t pid;
  int connFd;
} TSKM_SRV_CONNENT_t;

typedef struct {
  uint32_t num;
  TSKM_SRV_CONNENT_t conn[TSKM_COMM_CONNECT_MAX];
} TSKM_SRV_CONNENT_LIST_t;

typedef struct {
  int sockFd;
  const char *sockName;
  TSKM_SRV_CONNENT_LIST_t connList;
} TSKM_SRV_SOCK_CTX_t;

TSKM_ERR_t tskm_srvSockCreate(const char* sockName,
                              TSKM_SRV_SOCK_CTX_t* p_sock);
TSKM_SRV_CONNENT_t* tskm_srvSockConnect(TSKM_SRV_SOCK_CTX_t* p_sock);
void tskm_srvSockDisconnect(TSKM_SRV_SOCK_CTX_t* p_sock,
                            TSKM_SRV_CONNENT_t *p_conn);
void tskm_srvSockDestory(TSKM_SRV_SOCK_CTX_t* p_sock);
int tskm_cliSockConnect(const char* sockName);
int tskm_sockRcv(int fd, TSKM_EVENT_INFO_t* p_ev);
int tskm_sockSend(int fd, TSKM_EVENT_INFO_t* p_ev);
void tskm_sockDestory(int fd);

int tskm_comm_procInit(void);
int tskm_comm_setSvcWatchState(uint32_t id, BOOL bIsRun, uint32_t timeout);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_COMM_H_

