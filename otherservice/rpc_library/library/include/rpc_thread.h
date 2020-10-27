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

#ifndef OTHERSERVICE_RPCTHREAD_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_RPCTHREAD_H_  // NOLINT(build/header_guard)

#include <pthread.h>

/** @addtogroup RPClib_in
 * @{
 */

#define RPC_NO_SOCKET -1
#define RPC_NO_THREAD 0

#define RPC_MAIN_SUB_COMMAND "%-2d %-16lx"
#define RPC_MAIN_SUB_COMMAND_SIZE 20
#define RPC_MAIN_SUB_COMMANDs "%d %lx"
/** Type of command sent from the main thread to the subthread */
enum {
  RPC_COMMAND_ADD_SERVER, /* Addition of RPC_ID */
  RPC_COMMAND_REMOVE_SERVER, /* Remove RPC_ID */
  RPC_COMMAND_EXIT, /* Sub-thread termination (when all RPC_ID are exhausted) */
};

#define APICALL_TIMEOUT_DEFAULT -1

/** Periodic checking timeout (mSec) for detecting deadlocks */
#define TIMEOUT_FOR_DEADLOCK_CHECK 1000

/** Length of the socket name
  API request datagram socket = '\0' + "5-digit number associated with RPC_ID"
  Authentication stream socket  = '\0' + "secure_" + "5-digit number associated with RPC_ID" */
#define SOCK_NAME_LEN (1+5)        /* API request datagram socket */
#define SECURE_SOCK_NAME_LEN (1+12)    /* Authentication stream socket */

#define rpc_mutex_lock pthread_mutex_lock
#define rpc_mutex_unlock pthread_mutex_unlock

/** Upper limit of UID and GID list registration */
#define CREDENTIAL_LIST_NUM_MAX 32

/** Authentication result to be sent to the client */
typedef struct {
  UINT16 certify_res;  /**< Client Authentication Result  */
  pid_t srvr_pid;  /**< Server PID    */
} RpcCertifyResult;

#define CERTIFY_OK 0
#define CERTIFY_NG 1

/** Received API call request */
typedef struct {
  RPC_ID client;    /**< API ID of the requested program  */
  UINT16 api_num;    /**< Calling API number    */
  char *args_string;      /**< API Call Arguments    */
  UINT32 args_size;    /**< Number of bytes in the argument    */
} rpc_apicall_queue;

/** Source Client Socket Name Management Table */
typedef struct RpcClientSockNameInfoST RpcClientSockNameInfo;
struct RpcClientSockNameInfoST {
  char client_sock_name[SOCK_NAME_LEN];  /**< Socket Name List for Source Client  */
  pid_t pid;        /**< Source pid          */
  uid_t uid;        /**< Source uid          */
  gid_t gid;        /**< Source gid          */
  int wd;                               /**< Non-negative inotify monitored descriptors        */
  RpcClientSockNameInfo *next;  /**< Pointer to the next node      */
};

/** Communication server PID management table */
typedef struct RpcSrvrPidInfoST RpcSrvrPidInfo;
struct RpcSrvrPidInfoST {
  RPC_ID srvr_rpc_id;      /**< Communication destination server RPC_ID        */
  pid_t srvr_pid;      /**< Communication destination server pid        */
  int wd;        /**< Non-negative inotify monitored descriptors  */
  RpcSrvrPidInfo *next;    /**< Pointer to the next node            */
};

/** Buffer structure for API call acceptance processing.
 * This function is allocated only when the dispatch function is specified by RPC_start().
 */
typedef struct {
  RPC_dispatch_func_t dispatch_func;/**< Dispatch Functions to APIs (in stub) */
#define RPC_apicall_dispatch_func(id) \
  ((id)->apicall ? (id)->apicall->dispatch_func : 0)

  INT32 pipe_sub_main[2];/**< Notification pipe from the subthread to the main thread */
#define RPC_pipe_sub_main(th) ((th)->srvr_id->apicall->pipe_sub_main)

  UINT16 num_queue;/**< Number of queued API call requests */
#define RPC_apicall_num_queue(id) \
  ((id)->apicall ? (id)->apicall->num_queue : 0)
#define RPC_apicall_num_queue_inc(id) ((id)->apicall->num_queue++)
#define RPC_apicall_num_queue_dec(id) ((id)->apicall->num_queue--)

  rpc_apicall_queue queue[RPC_MAX_APICALL_QUEUE];/**< Queuing API Call Requests (FIFO) */
#define RPC_apicall_queue(id, i) ((id)->apicall->queue[(i)])
#define RPC_apicall_queue_client(id, i) ((id)->apicall->queue[(i)].client)
#define RPC_apicall_queue_api_num(id, i) ((id)->apicall->queue[(i)].api_num)
#define RPC_apicall_queue_args(id, i) ((id)->apicall->queue[(i)].args_string)
#define RPC_apicall_queue_args_size(id, i) ((id)->apicall->queue[(i)].args_size)

  RPC_ID in_process_client;/**< Clients running on ID */
#define RPC_apicall_in_process_client(id) ((id)->apicall->in_process_client)

  INT32 timeout_sec;/**< API processing timeout */
#define RPC_apicall_api_timeout_sec(id) ((id)->apicall->timeout_sec)

  INT32 secure_check;/**< Client Authentication Check Enabled/Disabled */
#define RPC_secure_check(id) ((id)->apicall->secure_check)

  INT32 regist_credential_info;/**< Registration of authentication information */
#define RPC_regist_credential_info(id) ((id)->apicall->regist_credential_info)

  uid_t *uid_list;/**< List of UIDs that can communicate */
#define RPC_uid_list(id, i) ((id)->apicall->uid_list[(i)])

  INT32 uid_num;/**< Number of UID list elements that can communicate */
#define RPC_uid_num(id) ((id)->apicall->uid_num)

  gid_t *gid_list;/**< GID list that can communicate */
#define RPC_gid_list(id, i) ((id)->apicall->gid_list[(i)])

  INT32 gid_num;/**< Number of GID list elements that can communicate */
#define RPC_gid_num(id) ((id)->apicall->gid_num)

  RpcClientSockNameInfo *sock_info_head;/**< Leading node of the source client's socket information management table */
#define RPC_sock_info_head(id) ((id)->apicall->sock_info_head)
#define RPC_client_sock_name(id) ((id)->apicall->sock_info_head->client_sock_name)
#define RPC_client_sock_pid(id) ((id)->apicall->sock_info_head->pid)
#define RPC_client_sock_next_node(id) ((id)->apicall->sock_info_head->next)

  INT32 client_sock_name_num;/**< Number of elements in the source client's socket name list */
#define RPC_client_sock_name_num(id) ((id)->apicall->client_sock_name_num)
#define RPC_client_sock_name_num_inc(id) ((id)->apicall->client_sock_name_num++)
#define RPC_client_sock_name_num_dec(id) ((id)->apicall->client_sock_name_num--)
} RpcApicallInfo;

struct RpcThreadInfo;

/** Structure that holds information about each RPC_ID */
typedef struct RpcIdInfo {
  struct RpcThreadInfo *thread_info;

  RPC_ID port;/**< Port number (=ID) used by the RPC library */
#define RPC_port(id) ((id)->port)
#define RPC_my_id(id) RPC_port(id)
#define rpc_get_port(id) (id)

  INT32 sock;/**< Sockets used by the RPC library */
#define RPC_my_sock(id) ((id)->sock)

  INT32 secure_sock;/**< Authentication socket used by the RPC library */
#define RPC_my_secure_sock(id) ((id)->secure_sock)

  int inotify_fd;/**< Server process monitoring inotify */
#define RPC_clnt_inotify_fd(id) ((id)->inotify_fd)

  INT32 count;/**< Number of clients using the same RPC_ID */
#define RPC_clnt_count(id) ((id)->count)
#define RPC_inc_clnt_count(id) ((id)->count++)
#define RPC_dec_clnt_count(id) ((id)->count--)

  RpcApicallInfo *apicall;/**< Information for API call acceptance processing */
#define RPC_apicall_info(id) ((id)->apicall)

  RpcSrvrPidInfo *srvr_pid_head;/**< Communication destination server PID management table top node */
#define RPC_srvr_pid_head(id) ((id)->srvr_pid_head)
#define RPC_srvr_rpc_id(id) ((id)->srvr_pid_head->srvr_rpc_id)
#define RPC_srvr_pid(id) ((id)->srvr_pid_head->srvr_pid)
#define RPC_srvr_pid_next_node(id) ((id)->srvr_pid_head->next)

  UINT32 return_str_len;/**<Number of bytes in the returned string as a result of an API call*/
  /* Including the terminating '\0' */
#define RPC_apicall_return_str_len(id) ((id)->return_str_len)

  UINT8 *return_str;/**<String returned as a result of an API call*/
#define RPC_apicall_return_str(id) ((id)->return_str)
} RpcIdInfo;

/** Received response packet */
typedef struct {
  RPC_ID id;/**< ID of the thread that sent the response */
  UINT16 type;/**< Response type */
  UINT32 seq_num;/**< The packet number to which this was sent (the response to this) */
} RpcResponse;

/** Structure that stores the state of each thread that called the RPC library */
typedef struct RpcThreadInfo {
  pthread_mutex_t th_mtx;/**< Mutex for modifying this struct */
#define RPC_THREAD_MUTEX_LOCK(th) (rpc_mutex_lock(&((th)->th_mtx)))
#define RPC_THREAD_MUTEX_UNLOCK(th) (rpc_mutex_unlock(&((th)->th_mtx)))

  pthread_t thread;/**< Thread calling the RPC library */
#define RPC_main_thread(th) ((th)->thread)

  UINT32 sequence_number;/**< Sequence number given to the transmitted packet */
  /* Send 30 times per second from the beginning of the thread, then wrap around two years.
   * It is not necessary to wrap around 0 because there is no comparison.
   */
  UINT32 magic;/**< Magic number to detect corrupted memories */

  RpcIdInfo  *srvr_id;/**< Information by RPC_ID (server) */
#define RPC_srvr_idinfo(th) ((th)->srvr_id)

  RpcIdInfo  *clnt_id;/**< Info by RPC_ID (client) */
#define RPC_clnt_idinfo(th) ((th)->clnt_id)
} RpcThreadInfo;

extern pthread_t g_rpc_thread;
extern UINT32 g_rpc_thread_alive;

#define PIPE_READ 0
#define PIPE_WRITE 1
extern int g_rpc_pipe_main_sub[2];

RpcThreadInfo *RpcMyThreadInfo(void);
RpcThreadInfo *RpcCreateThreadInfo(void);
int RpcCreateIdInfo(RpcThreadInfo *th, RPC_ID id,
    RPC_dispatch_func_t dispatch, INT32 secure_check);
void RpcDestroyIdInfo(RpcThreadInfo *th, RpcIdInfo *idinfo);
void RpcDestroyThreadInfo(void);
void RpcUnlinkSocketFiles(void);

/** @} */
#endif  // OTHERSERVICE_RPCTHREAD_H_
