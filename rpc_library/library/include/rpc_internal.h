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

/**
 * @file rpc_internal.h
 * @brief RPC Library Internal Implementation-Type, macro, and function definitions used internally
 *
 */
/** @addtogroup RPClib_in
 */
#ifndef OTHERSERVICE_RPCINTERNAL_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_RPCINTERNAL_H_  // NOLINT(build/header_guard)

#include <unistd.h>
#include <stdlib.h>

#if defined(__linux__)
/* Using unix autobind Features That Are Only Available in linux */
#define RPC_USE_UNIX_AUTOBIND
#endif /* __linux__ */

#define RPC_USE_SYSLOG

typedef RPC_ID *RPC_ID_p;

#include "rpc_udp.h"
#include "rpc_thread.h"

RPC_Result RpcQueueAPIRequestBefore(RpcIdInfo *id,
         UINT32 size, char **buff);
RPC_Result RpcQueueAPIRequestAfter(RpcIdInfo *id, RPC_ID client,
         const char *mesg, UINT32 size,
         char *args);
void RpcFreeAPIArgsString(char *args_string);
UINT16 RpcGetAPIRequest(RpcIdInfo *id, RPC_ID_p client,
      char **args_string, unsigned int *args_size);
RPC_Result RpcSetAPIcallReturn(RpcIdInfo *id,
          const char *mesg, UINT32 size);
void RpcDiscardAPIcallReturn(RpcIdInfo *id);

RPC_Result RpcDeleteSrvrPid(RpcIdInfo *idinfo, RPC_ID srvr_rpc_id, int wd);

#define StrEqual(a, b) (strcmp((a), (b)) == 0)
#define MemEqual(a, b, c) (memcmp((a), (b), (c)) == 0)

#define RPC_IS_INVALID_ID(a) ((a) == RPC_NO_ID)
#define RPC_INVALIDATE_ID(a) ((a) = RPC_NO_ID)
#define RPC_ID_COPY(a, b) ((a) = (b))
#define RPC_ID_Equal(a, b) ((a) == (b))

/* inotify Events read Sizes */
#define EVENT_SIZE  ( sizeof(struct inotify_event) )  /* One inotify event size */
#define BUF_LEN     (4 * EVENT_SIZE)  /* Read sizes */

#define LESS_THAN <

#define SET_NONBLOCK(fd) { \
    int flag; \
    flag = fcntl((fd), F_GETFL, 0); \
    fcntl((fd), F_SETFL, O_NONBLOCK|flag); \
  }

#define SET_CLOSE_ON_EXEC(fd) { \
    int flag; \
    flag = fcntl((fd), F_GETFD, 0); \
    fcntl((fd), F_SETFD, FD_CLOEXEC|flag); \
  }

#if defined(RPC_USE_SYSLOG)

#include <syslog.h>

#define RPC_LOG_CRIT(format, arg...)  syslog(LOG_CRIT, format "\n", ##arg)
#define RPC_LOG_ERR(format, arg...)   syslog(LOG_ERR, format "\n", ##arg)
#define RPC_LOG_STATE(format, arg...) syslog(LOG_INFO, format "\n", ##arg)
#define RPC_LOG_DEBUG(format, arg...) syslog(LOG_DEBUG, format "\n", ##arg)

#else  // defined(RPC_USE_SYSLOG)

#define RPC_LOG_CRIT(format, arg...)  fprintf(stderr, "[RPC:CRIT](%d): " format "\n", getpid(), ##arg)
#define RPC_LOG_ERR(format, arg...)   fprintf(stderr, "[RPC:ERR](%d): " format "\n", getpid(), ##arg)
#define RPC_LOG_STATE(format, arg...) fprintf(stderr, "[RPC:STATE](%d): " format "\n", getpid(), ##arg)
#define RPC_LOG_DEBUG(format, arg...)

#endif  // defined(RPC_USE_SYSLOG)

#include <sys/prctl.h>

#define RPC_LOG_ERR_W_NAME(format, arg...) \
do { \
  char name[16]; \
  prctl(PR_GET_NAME, name); \
  name[15] = '\0'; \
  RPC_LOG_ERR("(%s): " format, name, ##arg); \
} while (0)

#define RPC_LOG_PERROR(format, arg...) RPC_LOG_ERR("[%s:%d] %s : " format, __FILE__, __LINE__, strerror(errno), ##arg);

#include <assert.h>
#define rpc_assert assert

#define BUG_ASSERT(cond, x) \
  if (!(cond)) { \
    rpc_assert("BUG ASSERTION! " #x " @ " __FILE__ ==NULL); \
  }
#define CONFIG_ASSERT(s) rpc_assert((s) == NULL)

#define RUNS_IN_CALLERS_THREAD
#define RUNS_IN_READING_THREAD

#if !defined(RPC_USE_UNIX_AUTOBIND)

#define RPC_SOCKET_NAME "/tmp/RPC/%05d"

static inline void
rpc_set_socket_name(char *str, RPC_ID id) {
  sprintf(str, RPC_SOCKET_NAME, id);  // NOLINT (readability/nolint)
}

#endif /* !AUTOBIND */

#endif  // OTHERSERVICE_RPCINTERNAL_H_
