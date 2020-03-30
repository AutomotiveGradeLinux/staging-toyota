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
 * @file rpc_thread.c
 * @brief RPC Library Internal Implementation--Processing of Internally Generated Threads
 *
 */
/** @addtogroup RPClib_in */
/** @{ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/prctl.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <sys/inotify.h>

#include <other_service/rpc.h>
#include "rpc_internal.h"

#include <native_service/cl_monitor.h>
#include <native_service/cl_process.h>

/** Sub-threads that wake up in the RPC library */
static RpcThreadInfo *Thread_info[RPC_MAX_THREADS_IN_PROCESS];
static int Num_thread_info;
pthread_t g_rpc_thread = RPC_NO_THREAD;
UINT32 g_rpc_thread_alive;/**< Sub-thread running */

#define RPC_MAGIC_ID (('R'<<24)|('P'<<16)|('C'<<8)|'L')

/** Pipes used for communication with sub-threads
 * Read: Main thread, Write: Sub-thread */
int g_rpc_pipe_main_sub[2] = { -1, -1 };

static pthread_mutex_t process_global_mutex = PTHREAD_MUTEX_INITIALIZER;
#define PROCESS_MUTEX_LOCK rpc_mutex_lock(&process_global_mutex)
#define PROCESS_MUTEX_UNLOCK rpc_mutex_unlock(&process_global_mutex)

static void *RpcThreadMain(void *ptr);
static void NotifyMainThread(RpcThreadInfo *th);

static void KillRpcThread(void);
static void NotifyAddServer(RpcThreadInfo *th);
static void NotifyRemoveServer(RpcThreadInfo *th);

static RPC_Result RpcRegistSockName(RpcIdInfo *idinfo, char *client_sock_name, const struct ucred *cr, int wd);
static RPC_Result RpcCheckSockName(RpcIdInfo *idinfo, RPC_ID client_id);
static RPC_Result RpcDeleteSockName(RpcIdInfo *idinfo, int wd);
static RPC_Result RpcAllDeleteSockName(RpcIdInfo *idinfo, int inotify_fd);
static RPC_Result RpcCheckClientCredential(RpcIdInfo *idinfo, const struct ucred *cr);

#define RPC_SUB_THREAD_WAIT_SEC 5

#define WAIT_FOR_SUB_THREAD(loop_cond, sec) \
{ \
  struct timeval timeout; \
  timeout.tv_sec = sec; \
  timeout.tv_usec = 0; \
  \
  int fd = RPC_pipe_sub_main(th)[PIPE_READ]; \
  fd_set fds; \
  \
  while((loop_cond)) { \
    FD_ZERO(&fds); \
    FD_SET(fd, &fds); \
    int sret = select(fd + 1, &fds, NULL, NULL, &timeout); \
    if (sret < 0 && errno == EINTR) { \
      continue; \
    } else if (sret > 0 && FD_ISSET(fd, &fds)) { \
      char c; \
      read(fd, &c, sizeof(c)); \
    } else { \
      break; \
    } \
  } \
}

RUNS_IN_CALLERS_THREAD
  RpcThreadInfo *
RpcMyThreadInfo(void) {
  RpcThreadInfo *ret = NULL;
  int i;
  pthread_t me = pthread_self();

  PROCESS_MUTEX_LOCK;
  for(i = 0; i < RPC_MAX_THREADS_IN_PROCESS ; i++) {
    if (Thread_info[i] != NULL
        && pthread_equal(Thread_info[i]->thread, me)) {
      ret = Thread_info[i];
      break;
    }
  }
  PROCESS_MUTEX_UNLOCK;
  return ret;
}

RUNS_IN_CALLERS_THREAD
  RpcThreadInfo *
RpcCreateThreadInfo(void) {
  int i;
  pthread_t me = pthread_self();

  PROCESS_MUTEX_LOCK;
  /* Look for a free slot to store the thread info pointer */
  for(i = 0; i < RPC_MAX_THREADS_IN_PROCESS ; i++) {
    if (Thread_info[i] != NULL) {
      if (pthread_equal(Thread_info[i]->thread, me)) {  // LCOV_EXCL_BR_LINE 6: double check
        PROCESS_MUTEX_UNLOCK;  // LCOV_EXCL_START 6: double check
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        return Thread_info[i];
      }  // LCOV_EXCL_STOP
    } else {
      break;
    }
  }

  if (i == RPC_MAX_THREADS_IN_PROCESS) {
    PROCESS_MUTEX_UNLOCK;
    //CONFIG_ASSERT("Must increase RPC_MAX_THREADS_IN_PROCESS");
    RPC_LOG_ERR("Must increase RPC_MAX_THREADS_IN_PROCESS");
    return NULL;
  }

  /* Allocate area for thread info */
  // Because there is a timing when the server sub-thread is accessed without being initialized,
  // corrected so as to fill up to 0 in the MUTEX.
  RpcThreadInfo *th = rpc_malloc(sizeof(*th));
  if (th != NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    memset(th, 0, sizeof(*th));
    Thread_info[i] = th;
    th->magic = RPC_MAGIC_ID;
    Num_thread_info++;
  }
  PROCESS_MUTEX_UNLOCK;

  if (th == NULL) {  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_ERR("Can't alloc thread_info.");
    return NULL;
  }  // LCOV_EXCL_STOP

  /* Initializing Thread Info */
  th->thread = me;
  pthread_mutex_init(&(th->th_mtx), NULL);
  th->sequence_number = RPC_SEQ_NUM_START;

  return th;
}

/*
 * check if the allocated client ID conflicts with the server ID
 * of the same thread
 */
  static int
RpcCheckIdConflict(RpcThreadInfo *th, RPC_ID id) {
  RpcIdInfo *idinfo;
  idinfo = RPC_srvr_idinfo(th);
  if (idinfo != NULL && RPC_my_id(idinfo) == id) {  // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return 1;
  }  // LCOV_EXCL_STOP
  return 0;
}

/** Adding IDs with RPC_start()
 *
 * - Main: Create and initialize structures and tell sub-threads to add
 * - Sub: Add pointer to thread info (RpcThreadInfo) and notify it to the main process
 * - The main process waits for this procedure to finish.
 *   Use id_info->thread_info to determine completion (completed if not NULL)
 */
RUNS_IN_CALLERS_THREAD
  int
RpcCreateIdInfo(RpcThreadInfo *th,
    RPC_ID id, RPC_dispatch_func_t dispatch, INT32 secure_check) {
  RpcIdInfo *id_info = rpc_malloc(sizeof(*id_info));
  if (id_info == NULL) {  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;
  }  // LCOV_EXCL_STOP
  memset(id_info, 0, sizeof(*id_info));

  /*
   * Creates a Unix domain socket based on a given number of ports
   */
#if defined(RPC_USE_UNIX_AUTOBIND)
  int sock_un = -1, secure_sock_un = -1;
  socklen_t sa_len;
  struct sockaddr_un sa_un;

get_id_retry:
  sock_un = socket(PF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0);  /* Datagram socket for receiving API requests */
  if (sock_un < 0) {
    RPC_LOG_PERROR("socket");
    goto error;
  }
  SET_NONBLOCK(sock_un);
  SET_CLOSE_ON_EXEC(sock_un);

  /*
   * Naming Rules for Unix domain Sockets
   * Server:(ID=50000-59999)
   *   sa_un.sun_path[0] = 0x00;
   *   sa_un.sun_path[1] = 'S';
   *   sa_un.sun_path[2-5] = sprintf("%04x", ID);
   *
   * Client:(ID=1-0xfffff)
   *   sa_un.sun_path[0] = 0x00;
   *   sa_un.sun_path[1-5] = sprintf("%05x", ID);
   *   ID is autobind by kernel during bind(see linux/net/unix/af_unix.c)
   *   ! Since it depends on the unix socket implementations of Linux, be careful when porting to other operating systems.
   *
   * ID=50000-59999 is duplicated in Server and Client, 
   * but generated it according to the above rules when sent in the RPClib (see rpc_udp.c)
   *
   * Because file deletion is leaked when the system is forcibly terminated and abnormal process termination 
   * by a traditional way to create and bind files under /tmp/RPC/,
   * change to the above method(2009.02.04,2012.01.21)
   */
  memset(&sa_un, 0, sizeof(sa_un));
  sa_un.sun_family = AF_UNIX;
  if (dispatch != NULL) {  // server
    RpcSetServerName(sa_un.sun_path, id);
    sa_len = sizeof(sa_un.sun_family) + SOCK_NAME_LEN;
  } else {  // client
    // Automatically assign name (ID) by unix_autobind()
    sa_len = sizeof(sa_un.sun_family);
  }

#else /* !AUTOBIND */
  int sock_un = -1;
  struct sockaddr_un sa_un;
  sa_un.sun_family = AF_UNIX;
  rpc_set_socket_name(sa_un.sun_path, id);
  sock_un = socket(PF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0);
  if (sock_un < 0) {
    RPC_LOG_PERROR("socket(unix)");
    goto error;
  }
  SET_NONBLOCK(sock_un);
  SET_CLOSE_ON_EXEC(sock_un);
  unlink(sa_un.sun_path);
  sa_len = sizeof(sa_un);
#endif  /* !AUTOBIND */
  if (bind(sock_un, (struct sockaddr *)&sa_un, sa_len) < 0) {
    RPC_LOG_PERROR("DGRAM : bind(unix), ID:%#x", id);
    goto error;
  }

#if defined(RPC_USE_UNIX_AUTOBIND)
  if (dispatch == NULL) {  // client
    // Retrieves the assigned name (ID)
    socklen_t len = sizeof(sa_un);
    if (getsockname(sock_un, (struct sockaddr *)&sa_un, &len) < 0) {
      perror("getsockname");
      goto error;
    }
    RpcGetClientName(sa_un.sun_path, &id);
    if (RpcCheckIdConflict(th, id)) {  // LCOV_EXCL_BR_LINE 8: dead code, RpcCheckIdConflict always is false
      // LCOV_EXCL_START 8: dead code, RpcCheckIdConflict always is false
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      RPC_LOG_STATE("ID %d conflicts with server -- get next one", id);
      close(sock_un);
      goto get_id_retry;
    }  // LCOV_EXCL_STOP
    RPC_LOG_DEBUG("client %s", sa_un.sun_path + 1);
  }
#endif /* AUTOBIND */

  id_info->port = id;
  id_info->sock = sock_un;

  if (dispatch != NULL) {  /* server */
    rpc_assert(th->srvr_id == NULL);  // LCOV_EXCL_BR_LINE 6: double check
    RpcApicallInfo *apicall = rpc_malloc(sizeof(*apicall));
    if (apicall == NULL) {  // LCOV_EXCL_START 5: fail safe for libc malloc
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      goto error;
    }  // LCOV_EXCL_STOP

    /* Create Socket for Authentication */
    socklen_t secure_sa_len;
    struct sockaddr_un secure_sa_un;

    secure_sock_un = socket(PF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);  /* stream socket for authentication */
    if (secure_sock_un < 0) {
      RPC_LOG_PERROR("socket");
      rpc_free(apicall);
      apicall = NULL;
      goto error;
    }
    SET_NONBLOCK(secure_sock_un);
    SET_CLOSE_ON_EXEC(secure_sock_un);

    memset(&secure_sa_un, 0, sizeof(secure_sa_un));
    secure_sa_un.sun_family = AF_UNIX;
    RpcSetServerSecureName(secure_sa_un.sun_path, id);
    secure_sa_len = sizeof(secure_sa_un.sun_family) + SECURE_SOCK_NAME_LEN;

    /* Bind socket for authentication */
    if (bind(secure_sock_un, (struct sockaddr *)&secure_sa_un, secure_sa_len) < 0) {
      RPC_LOG_PERROR("STREAM : bind(unix), ID:%#x", id);
      rpc_free(apicall);
      apicall = NULL;
      goto error;
    }

    id_info->secure_sock = secure_sock_un;

    memset(apicall, 0, sizeof(*apicall));
    apicall->dispatch_func = dispatch;
    apicall->pipe_sub_main[PIPE_READ] = -1;
    apicall->pipe_sub_main[PIPE_WRITE] = -1;
    apicall->timeout_sec = 30;  /* Server API processing timeout */
    apicall->secure_check = secure_check;  /* Authentication check by UID list */
    if (NEED_SECURE_CHECK == secure_check) {  /* Initializes the UID list with not-registered if secured given. */
      apicall->regist_credential_info = NO_REGISTERED;
    }
    apicall->sock_info_head = NULL;  /* Leading Node of Source Client Socket Info */
    apicall->client_sock_name_num = 0;  /* Number of elements in the source client's socket name list */
    apicall->in_process_client = RPC_NO_PORT;  /* Client RPC_ID during API processing */

    id_info->apicall = apicall;
    th->srvr_id = id_info;

    /* Creating a pipe for communication pipe from sub-thread to main-thread direction */
    if (pipe(apicall->pipe_sub_main) != 0) {
      RPC_LOG_PERROR("pipe");
      goto error;
    }
    SET_NONBLOCK(apicall->pipe_sub_main[PIPE_READ]);
    SET_CLOSE_ON_EXEC(apicall->pipe_sub_main[PIPE_READ]);
    SET_NONBLOCK(apicall->pipe_sub_main[PIPE_WRITE]);
    SET_CLOSE_ON_EXEC(apicall->pipe_sub_main[PIPE_WRITE]);

    PROCESS_MUTEX_LOCK;
    if (g_rpc_thread == RPC_NO_THREAD) {  /* There are no sub-threads. */
      /* Creating a pipe for communication from main-thread to sub-thread direction */
      if (pipe(g_rpc_pipe_main_sub) != 0) {
        RPC_LOG_PERROR("pipe");
        PROCESS_MUTEX_UNLOCK;
        goto error;
      }

      SET_NONBLOCK(g_rpc_pipe_main_sub[PIPE_READ]);
      SET_CLOSE_ON_EXEC(g_rpc_pipe_main_sub[PIPE_READ]);
      SET_NONBLOCK(g_rpc_pipe_main_sub[PIPE_WRITE]);
      SET_CLOSE_ON_EXEC(g_rpc_pipe_main_sub[PIPE_WRITE]);

      /* Creating sub-thread */
      pthread_t read_th;
      if (pthread_create(&read_th, NULL, RpcThreadMain, 0) != 0) {
        RPC_LOG_PERROR("pthread_create");
        PROCESS_MUTEX_UNLOCK;
        goto error;
      }
      g_rpc_thread = read_th;
    }
    PROCESS_MUTEX_UNLOCK;

    /* Instruct a sub-thread to add and wait for completion */
    NotifyAddServer(th);
    // LCOV_EXCL_BR_START 15: macro define in rpc_thread.c
    WAIT_FOR_SUB_THREAD((th->srvr_id->thread_info == NULL),
        RPC_SUB_THREAD_WAIT_SEC);
    // LCOV_EXCL_BR_STOP
    rpc_assert(th->srvr_id->thread_info != NULL);  // LCOV_EXCL_BR_LINE 6: double check

  } else {  /* dispatch == NULL => client */
    id_info->count = 1;
    id_info->thread_info = th;
    th->clnt_id = id_info;
  }
  return 0;  /* pgr0524 */

error:
  if (g_rpc_pipe_main_sub[PIPE_READ] >= 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc socket
    close(g_rpc_pipe_main_sub[PIPE_READ]);
  }
  if (g_rpc_pipe_main_sub[PIPE_WRITE] >= 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc socket
    close(g_rpc_pipe_main_sub[PIPE_WRITE]);
  }
  if (id_info->apicall != NULL) {
    if (id_info->apicall->pipe_sub_main[PIPE_READ] >= 0) {
      close(id_info->apicall->pipe_sub_main[PIPE_READ]);
    }
    if (id_info->apicall->pipe_sub_main[PIPE_WRITE] >= 0) {
      close(id_info->apicall->pipe_sub_main[PIPE_WRITE]);
    }
    rpc_free(id_info->apicall);
  }
  if (sock_un != -1) {
    close(sock_un);
#if !defined(RPC_USE_UNIX_AUTOBIND)
    unlink(sa_un.sun_path);
#endif  /* !AUTOBIND */
  }
  if (secure_sock_un != -1) {
    close(secure_sock_un);
#if !defined(RPC_USE_UNIX_AUTOBIND)
    unlink(secure_sa_un.sun_path);
#endif  /* !AUTOBIND */
  }
  rpc_free(id_info);
  th->srvr_id = NULL;
  return -1;  /* pgr0524 */
}

/*
 * Notify an unfinished request of an error at server termination (RPC_end).
 */
  static void
RpcSendErrorToPendingRequest(RpcThreadInfo *th, RpcIdInfo *idinfo) {
  UINT16 api_num;
  RPC_ID client;
  char *args_string;
  unsigned int args_size;
  rpc_send_buf sendbuf;
  char retcode[10];

  do {
    api_num = RpcGetAPIRequest(idinfo, &client,
        &args_string, &args_size);
    if (api_num > 0) {  /* API calls are queued */

      sprintf(retcode, "%08x ", RPC_ERR_Fatal);
      sendbuf.buf = retcode;
      sendbuf.bytes = sizeof(retcode) - 1;
      RpcSendUdp2(idinfo, client, RPC_SEND_TO_CLIENT,
          RPC_PACKET_APIRETURN, 1, &sendbuf);
      RPC_LOG_STATE("sent error result to pending client %05x", client);
      RpcFreeAPIArgsString(args_string);
    }
  } while(api_num > 0);
}

/*
 * Notify unfinished request of deadlock when deadlock of the server is detected.
 */
  static void
RpcSendDeadlockToPendingRequest(RpcThreadInfo *th, RpcIdInfo *idinfo) {
  UINT16 api_num;
  RPC_ID client;
  char *args_string;
  unsigned int args_size;
  rpc_send_buf sendbuf;
  char retcode[10];

  do {
    api_num = RpcGetAPIRequest(idinfo, &client,
        &args_string, &args_size);
    if (api_num > 0) {  /* API calls are queued */

      sprintf(retcode, "%08x ", RPC_ERR_Server_DeadLock);
      sendbuf.buf = retcode;
      sendbuf.bytes = sizeof(retcode) - 1;
      RpcSendUdp2(idinfo, client, RPC_SEND_TO_CLIENT,
          RPC_PACKET_APIRETURN, 1, &sendbuf);
      RPC_LOG_STATE("sent deadlock result to pending client %05x", client);
      RpcFreeAPIArgsString(args_string);
    }
  } while(api_num > 0);

  if (RPC_NO_PORT != RPC_apicall_in_process_client(idinfo)) {
    sprintf(retcode, "%08x ", RPC_ERR_Server_DeadLock);
    sendbuf.buf = retcode;
    sendbuf.bytes = sizeof(retcode) - 1;
    RpcSendUdp2(idinfo, RPC_apicall_in_process_client(idinfo), RPC_SEND_TO_CLIENT,
        RPC_PACKET_APIRETURN, 1, &sendbuf);
    RPC_LOG_STATE("sent deadlock result to pending client %05x", RPC_apicall_in_process_client(idinfo));
    RPC_apicall_in_process_client(idinfo) = RPC_NO_PORT;
  }
}

/** Delete RPC_ID Info by RPC_end()
 *
 * - Main: Notify sub of deletion of RPC_ID info.
 * - Sub: Delete a pointer from thread info (RpcThreadInfo) and notify main of that.
 * - Main waits for this procedure to finish.
 *   Use id_info->thread_info to determine completion (completed if NULL).
 *   Then, release the memory related to RPC_ID info and close the socket.
 */
RUNS_IN_CALLERS_THREAD
  void
RpcDestroyIdInfo(RpcThreadInfo *th, RpcIdInfo *id_info) {
  rpc_assert(id_info->count == 0);  // LCOV_EXCL_BR_LINE 6: double check
  if (id_info->apicall) {
    if (g_rpc_thread_alive != 0) {
      NotifyRemoveServer(th);
      /* Wait for a sub-thread to recognize IDinfo deletion */
      // LCOV_EXCL_BR_LINE 15: macro define in rpc_thread.c
      WAIT_FOR_SUB_THREAD((th->srvr_id->thread_info != NULL),
          RPC_SUB_THREAD_WAIT_SEC);
      // LCOV_EXCL_BR_STOP
      rpc_assert(th->srvr_id->thread_info == NULL);  /* not recognized yet */  // LCOV_EXCL_BR_LINE 6: double check
    }

    if (id_info->apicall->pipe_sub_main[PIPE_READ] >= 0) {
      close(id_info->apicall->pipe_sub_main[PIPE_READ]);
    }
    if (id_info->apicall->pipe_sub_main[PIPE_WRITE] >= 0) {
      close(id_info->apicall->pipe_sub_main[PIPE_WRITE]);
    }
    if (id_info->secure_sock >= 0) {
      close(id_info->secure_sock);
    }
    rpc_free(id_info->apicall);
    th->srvr_id = NULL;
  } else {
    th->clnt_id = NULL;
  }
  rpc_free(id_info);
}

RUNS_IN_CALLERS_THREAD
  void
RpcDestroyThreadInfo(void) {
  unsigned int i;
  RpcThreadInfo *th = RpcMyThreadInfo();
  if (th == NULL) {
    return;
  }

  RPC_THREAD_MUTEX_LOCK(th);
  if (th->thread == RPC_NO_THREAD) {  // LCOV_EXCL_BR_LINE 6: double check
    RPC_THREAD_MUTEX_UNLOCK(th);  // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;
  }  // LCOV_EXCL_STOP
  th->thread = RPC_NO_THREAD;
  RPC_THREAD_MUTEX_UNLOCK(th);

  PROCESS_MUTEX_LOCK;
  /*
   * Remove the pointer from the global variable.
   * Subsequent calls to RpcMyThreadInfo() return NULL
   */
  for(i = 0; i < RPC_MAX_THREADS_IN_PROCESS ; i++) {
    if (th == Thread_info[i]) {
      Thread_info[i] = NULL;
      Num_thread_info--;
      break;
    }
  }
  PROCESS_MUTEX_UNLOCK;
  BUG_ASSERT(i < RPC_MAX_THREADS_IN_PROCESS, "No info in Thread_info[]");  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_internal.h

  if (Num_thread_info == 0 && g_rpc_thread_alive != 0) {
    KillRpcThread();
    char name[32];
    prctl(PR_GET_NAME, name);
    RPC_LOG_DEBUG("[%s]waiting for sub thread to join...", name);
    pthread_join(g_rpc_thread, NULL);
    RPC_LOG_DEBUG("[%s]sub thread joined.", name);
    g_rpc_thread = RPC_NO_THREAD;  /* bug fix */
    rpc_assert(g_rpc_thread_alive == 0);  // LCOV_EXCL_BR_LINE 6: double check
    close(g_rpc_pipe_main_sub[PIPE_READ]);
    close(g_rpc_pipe_main_sub[PIPE_WRITE]);
  }

  if (th->srvr_id != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
    RpcDestroyIdInfo(th, th->srvr_id);  // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    th->srvr_id = NULL;
  }  // LCOV_EXCL_STOP
  if (th->clnt_id != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
    RpcDestroyIdInfo(th, th->clnt_id);  // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    th->clnt_id = NULL;
  }  // LCOV_EXCL_STOP

  pthread_mutex_destroy(&(th->th_mtx));
  rpc_free(th);
}

#if !defined(RPC_USE_UNIX_AUTOBIND)
/**
 * Sub-function of RPC_end_all()
 *
 * Assuming releasing memory and closing the socket are processing at immediately after the end of the process,
 * Suppress socket file leaks by only deleting socket files because to avoid deadlocks and shorten the time
 * by exclusive processing.
 */
  void
RpcUnlinkSocketFiles(void) {
  int i;
  char sock_name[16];

  for(i = 0; i < RPC_MAX_THREADS_IN_PROCESS ; i++) {
    RpcThreadInfo *th = Thread_info[i];
    if (th != NULL) {
      if (th->srvr_id != NULL) {
        rpc_set_socket_name(sock_name, RPC_port(th->srvr_id));
        RPC_LOG_STATE("unlink srvr %s", sock_name);
        unlink(sock_name);
      }
      if (th->clnt_id != NULL) {
        rpc_set_socket_name(sock_name, RPC_port(th->clnt_id));
        RPC_LOG_STATE("unlink clnt %s", sock_name);
        unlink(sock_name);
      }
    }
  }
}
#endif /* !AUTOBIND */

/*
 * Deadlock detection check for servers in the thread
 */
  static void
RpcDeadlockCheck(RpcThreadInfo** thread_info, unsigned int num_thread_info) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  unsigned int i = 0;
  for(i = 0 ; i < num_thread_info ; i++) {
    RpcThreadInfo *th = thread_info[i];
    RpcIdInfo *idinfo = th->srvr_id;
    CL_MonitorEntry_t entry;

    if (0 == CL_MonitorGetEntry(CL_MONITOR_TYPE_RPC, RPC_port(idinfo), &entry)) {
      if (entry.state == CL_MONITOR_STATE_RUN && entry.timeout <= ts.tv_sec) {
        RPC_LOG_ERR("Time Out : RPC_ID = %#x API_NUM = %#x", entry.id, entry.user_data);
        fprintf(stderr, "Time Out : RPC_ID = %#x API_NUM = %#x\n", entry.id, entry.user_data);
        RpcSendDeadlockToPendingRequest(th, idinfo);
      }
    }
  }
}
/** Main functions of the sub-threads (READING THREAD)
*/
RUNS_IN_READING_THREAD
  static void *
RpcThreadMain(void *ptr __attribute__((unused))) {
  struct pollfd wait_files[RPC_MAX_FD_IN_PROCESS];
  RpcThreadInfo *thread_info[RPC_MAX_THREADS_IN_PROCESS];
  unsigned int num_thread_info = 0;

  unsigned int poll_num;
  int need_reset_sockfd = 1;
  int normal_exit = 0;
  RPC_Result result;
  unsigned int i, j;

  /* Monitoring for clients process with inotify() *//* Monitoring target filename */
  const int inotify_fd = inotify_init1(IN_CLOEXEC);  /* fd for process monitoring with inotify() */

  UINT8 readbuf[RPC_UDP_PACKET_SIZE];
  memset(readbuf, 0, sizeof(UINT8) * RPC_UDP_PACKET_SIZE);

  g_rpc_thread_alive = 1;

  CL_MonitorInit(CL_MONITOR_INIT_USER);  /* Using the API for Error Monitoring */

  // Name the thread created in the RPClib (append "_R")
#define RPC_APPEND_NAME "_R"
#ifndef PRF_SIZE_PROCESSNAME
#define PRF_SIZE_PROCESSNAME     8  /* Limit name length for Profiler Analysis Tools */
#endif
  {
    char *p, name[32];
    prctl(PR_GET_NAME, name);
    name[PRF_SIZE_PROCESSNAME] = '\0';
    if (strlen(name) + strlen(RPC_APPEND_NAME) > PRF_SIZE_PROCESSNAME) {
      p = name + PRF_SIZE_PROCESSNAME - strlen(RPC_APPEND_NAME);
    } else {
      p = name + strlen(name);
    }
    strcpy(p, RPC_APPEND_NAME);
    prctl(PR_SET_NAME, name);
  }

  /* Set the communication pipe with the main thread to poll fd */
  poll_num = 1;
  wait_files[0].fd = g_rpc_pipe_main_sub[PIPE_READ];
  wait_files[0].events = POLLIN;

restart:
  for( ; ; ) {
    if (need_reset_sockfd) {
      /* Set the UDP socket of each RPC_ID to poll fd */
      PROCESS_MUTEX_LOCK;
      for(i = 0, j = 0 ; i < RPC_MAX_THREADS_IN_PROCESS ; i++) {
        if (Thread_info[i] != NULL) {
          if (Thread_info[i]->magic != RPC_MAGIC_ID) {  // LCOV_EXCL_BR_LINE 6: double check
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            RPC_LOG_ERR("Someone(me?) destroyed my area!");  // LCOV_EXCL_LINE 6: double check
          }
          if (Thread_info[i]->srvr_id != NULL
              && Thread_info[i]->srvr_id->thread_info != NULL) {
            thread_info[j] = Thread_info[i];
            j++;
          }
        }
      }
      PROCESS_MUTEX_UNLOCK;
      num_thread_info = j;

      poll_num = 1;
      /* Register fd for monitoring with inotify() in poll() */
      wait_files[1].fd = inotify_fd;
      wait_files[1].events = POLLIN;

      poll_num = 2;
      for(i = 0 ; i < num_thread_info ; i++) {
        /* Datagram socket for API request */
        wait_files[poll_num].fd = thread_info[i]->srvr_id->sock;  /* pgr0000 */
        wait_files[poll_num].events = POLLIN;
        poll_num++;
        // LCOV_EXCL_BR_START 5: fail safe for libc listen
        /* Authentication stream socket */
        if (0 != listen(thread_info[i]->srvr_id->secure_sock, 10)) {  /* Number of queues */
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          RPC_LOG_PERROR("listen(unix)");  // LCOV_EXCL_LINE 5: fail safe for libc listen
        }
        // LCOV_EXCL_BR_STOP
        wait_files[poll_num].fd = thread_info[i]->srvr_id->secure_sock;
        wait_files[poll_num].events = POLLIN;
        poll_num++;
      }
      need_reset_sockfd = 0;
    }

    int pollret;
    pollret = poll(wait_files, poll_num, TIMEOUT_FOR_DEADLOCK_CHECK);

    int save_errno = errno;

    RpcDeadlockCheck(thread_info, num_thread_info);
    if (pollret < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc poll
      if (save_errno == EINTR) {  // LCOV_EXCL_START 5: fail safe for libc poll
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        continue;
      } else {
        rpc_assert(pollret > 0);
        goto exit_read_thread;
      }  // LCOV_EXCL_STOP
    }

    /* Commands from the main thread (via pipe) */
    if ((wait_files[0].revents & POLLIN) == POLLIN) {
      char buf[RPC_MAIN_SUB_COMMAND_SIZE];
      long ret, arg;
      int cmd;
      RpcThreadInfo *th;
      ret = read(wait_files[0].fd, buf, sizeof(buf));
      if (ret == sizeof(buf)) {
        sscanf(buf, RPC_MAIN_SUB_COMMANDs, &cmd, &arg);
        switch(cmd) {
          case RPC_COMMAND_ADD_SERVER:
            th = (RpcThreadInfo *)arg;
            th->srvr_id->thread_info = th;  /* Indicate the completion of the processing */
            NotifyMainThread(th);
            need_reset_sockfd = 1;
            goto restart;
            break;

          case RPC_COMMAND_REMOVE_SERVER:
            th = (RpcThreadInfo *)arg;
            RpcSendErrorToPendingRequest(th, th->srvr_id);

            RpcAllDeleteSockName(th->srvr_id, inotify_fd);  /* delete client_sock_name_list */
            rpc_free((th->srvr_id)->apicall->uid_list);  /* delete uid_list */
            rpc_free((th->srvr_id)->apicall->gid_list);  /* delete gid_list */

            th->srvr_id->thread_info = NULL;/* Indicate the completion of the processing */
            NotifyMainThread(th);
            need_reset_sockfd = 1;
            goto restart;
            break;

          case RPC_COMMAND_EXIT:
            /************ Termination request from the parent thread *************/
            RPC_LOG_DEBUG("Received exit command from main thread.");
            normal_exit = 1;
            goto exit_read_thread;
            break;
        }  /* switch */
      }  /* if (ret == sizeof(buf)) */
    }  /* Complete the processing of commands from the main thread */

    /* Client Monitoring Events with inotify() */
    if ((wait_files[1].revents & POLLIN) == POLLIN) {
      UINT32 read_len = 0;
      int length = 0;
      char *buffer;
      buffer = (char *)rpc_malloc(BUF_LEN);
      if (NULL == buffer) {// LCOV_EXCL_START 5: fail safe for libc malloc
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        RPC_LOG_ERR("rpc_malloc() ERROR.");
        goto exit_read_thread;
      }  // LCOV_EXCL_STOP

      if( (length = (int)read( inotify_fd, buffer, BUF_LEN ) ) < 0 ) {  // LCOV_EXCL_START 5: fail safe for libc read
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        RPC_LOG_PERROR("inotify read() ERROR.");
        rpc_free(buffer);
        buffer = NULL;
        goto exit_read_thread;
      }  // LCOV_EXCL_STOP
      while ( read_len < length ) {
        struct inotify_event *event = ( struct inotify_event * )&buffer[read_len];

        if ( event->mask & IN_DELETE_SELF ) {/* Terminating a Client Process */
          int i;                  /* Looping variable */

          /* Delete the source socket name from all RpcThreadInfo in the received thread */
          for(i = 0 ; i < num_thread_info ; i++) {
            RpcThreadInfo *th = thread_info[i];
            RpcDeleteSockName(th->srvr_id, event->wd);
          }
        }
        read_len += (UINT32)(EVENT_SIZE + event->len);  /* Size of the inotify_event structure */
      }
      rpc_free(buffer);
      goto restart;
    }  /* Client Monitoring Events Completed with inotify() */

    for(i = 2 ; i < poll_num ; i++) {
      /* Event to the API request datagram socket */
      if ((i % 2 == 0) && ((wait_files[i].revents & POLLIN) == POLLIN)) {
        unsigned int thread_info_num = ((i/2) - 1);  /* Compute thread_info[thread_info_num] with events */
        RpcThreadInfo *th = thread_info[thread_info_num];  /* pgr0000 */
        RpcIdInfo *idinfo = th->srvr_id;
        for(;;) {
          /* RPClib packet received */
          int readret = RpcReadUdpPacket(idinfo, readbuf);
          if (readret < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc recvfrom
            rpc_assert(readret >= 0);  // LCOV_EXCL_START 5: fail safe for libc recvfrom
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            goto exit_read_thread;
            // LCOV_EXCL_STOP
          } else if (readret == 0) {
            break;
          }
          /* successfully read udp packets */
          /* parse the packet and queue events */
          RPC_ID sender = RPC_NO_ID;
          UINT32 seq_num = 0;
          UINT32 size = 0;
          RPC_packet_type command = RPC_PACKET_NONE;
          if (RpcParsePacketHeader((const char *)readbuf, &command, &sender, &seq_num, &size) != RPC_OK) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
            goto exit_read_thread;
          }

          long int api_num;
          char *buff;

          switch(command) {
            case RPC_PACKET_APICALL:
              if (RPC_DEBUG != NULL) {  // LCOV_EXCL_BR_LINE 7: debug
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                printf("RPC[%s]: received APIcall\n", RPC_DEBUG);  // LCOV_EXCL_LINE 7: debug
              }
              // Return response without queuing for ALIVE query
              api_num = strtol((const char *)(readbuf + RPC_PACKET_HEADER_LEN), NULL, 10);
              if (api_num == RPC_API_NUM_RPC_ALIVE) {
                RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                    RPC_RESPONSE_APICALL,
                    seq_num, "OK", sizeof("OK"));
                break;
              }

              /* Return BUSY if secure and unregistered in UID-list */
              if ((NEED_SECURE_CHECK == RPC_secure_check(idinfo))
                    && (NO_REGISTERED == RPC_regist_credential_info(idinfo))) {
                RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                RPC_RESPONSE_APICALL,
                seq_num, "BUSY", sizeof("BUSY"));
                RPC_LOG_ERR("Need UID list register.");
                break;
              }

              result = RpcQueueAPIRequestBefore(idinfo, size, (char**)&buff);
              if (result == RPC_OK) {
                /* Check whether the source has been authenticated */
                if(RPC_OK == RpcCheckSockName(idinfo, sender)) {  /* Registerd the name of the source socket */
                  RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                      RPC_RESPONSE_APICALL,
                      seq_num, "OK", sizeof("OK"));
                  RpcQueueAPIRequestAfter(idinfo, sender,
                      (const char *)(readbuf + RPC_PACKET_HEADER_LEN),
                      size, buff);
                } else {  /* Not registered (in other words, first communication with the source client) */
                  /* Authentication request to the client */
                  RPC_THREAD_MUTEX_UNLOCK(idinfo->thread_info);
                  RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                      RPC_RESPONSE_APICALL,
                      seq_num, "CERT", sizeof("CERT"));
                  rpc_free(buff);
                  break;
                }
              } else if (result == RPC_ERR_Busy) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
                RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                    RPC_RESPONSE_APICALL,
                    seq_num, "BUSY", sizeof("BUSY"));
              } else {  // LCOV_EXCL_START 5: fail safe for libc malloc
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                RpcSendUdpResponse(idinfo, sender, RPC_SEND_TO_CLIENT,
                    RPC_RESPONSE_APICALL,
                    seq_num, "ERR", sizeof("ERR"));
                RPC_LOG_ERR("queueing APIcall failed.(%d)", result);
                goto exit_read_thread;
              }  // LCOV_EXCL_STOP
              NotifyMainThread(th);
              if (RPC_DEBUG != NULL) {  // LCOV_EXCL_BR_LINE 7: debug
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                printf("RPC[%s]: notified APIcall\n", RPC_DEBUG);  // LCOV_EXCL_LINE 7: debug
              }
              break;

            default:
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              BUG_ASSERT(0, "Unknown UDP packet type");  // LCOV_EXCL_LINE 15: marco defined in rpc_internal.h
              goto exit_read_thread;
              break;
          } /* switch(command) */
        }/* for(;;) */

        /* Event to the stream socket for authentication */
      } else if ((i % 2 != 0) && ((wait_files[i].revents & POLLIN) == POLLIN)) {
        unsigned int thread_info_num = ((i-1)/2 - 1);  /* Compute thread_info[thread_info_num] with events */
        struct sockaddr_un client_sa_un;
        socklen_t client_len = sizeof(struct sockaddr_un);
        struct ucred cr;  /* Structure of client credit info */
        RpcCertifyResult send_ret;  /* Authentication result to pass to the client */
        RpcThreadInfo *th = thread_info[thread_info_num];
        RpcIdInfo *idinfo = th->srvr_id;

        send_ret.certify_res = CERTIFY_NG;
        send_ret.srvr_pid = 0;

        /* Obtain client credit info from a connected socket */
        int accept_sock = accept4(wait_files[i].fd, (struct sockaddr *)&client_sa_un, &client_len, SOCK_CLOEXEC);
        int ret = getsockopt(accept_sock, SOL_SOCKET, SO_PEERCRED, &cr, &client_len);
        if (ret == 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc getsockopt
          client_sa_un = (struct sockaddr_un )client_sa_un;
          /* Check if UID of client is allowed to communicate */
          if (RPC_OK == RpcCheckClientCredential(idinfo, &cr)) {

            /* Obtain the socket name associated with the RPC_ID of the client from the socket info */
            char client_sock_name[SOCK_NAME_LEN];
            RpcGetClientNameFromSock(client_sa_un.sun_path, client_sock_name);

            /* Monitoring client processes with inotify */
            char intfy_fname[32];
            snprintf(intfy_fname, sizeof(intfy_fname), CL_INTFY_FILENAME_FORMAT, cr.pid);
            int wd = inotify_add_watch(inotify_fd, intfy_fname, IN_DELETE_SELF);
            if (0 > wd) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc inotify_add_watch
              RPC_LOG_STATE("intfy_fname is Not Found [%s].", intfy_fname);
            }

            /* Register the source socket name in the management table */
            RpcRegistSockName(idinfo, client_sock_name, &cr, wd);

            /* Send server credit info to the client */
            send_ret.certify_res = CERTIFY_OK;
            send_ret.srvr_pid = getpid();
          }
        }

        /* Send authentication result to client */
        send(accept_sock, (char*)&send_ret, sizeof(RpcCertifyResult), 0);
        close(accept_sock);

        goto restart;

      } else if ((wait_files[i].revents & ~POLLIN) != 0) {  // LCOV_EXCL_START 5: fail safe for libc poll
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        // POLLERR, etc. on UDP port
        RPC_LOG_STATE("poll error %x", wait_files[i].revents);

        if ((wait_files[i].revents & POLLNVAL) == POLLNVAL) {
          need_reset_sockfd = 1;
          goto restart;
        }
      }  /* if ((wait_files[i].revents & POLLIN) == POLLIN) */  // LCOV_EXCL_STOP

    }  /* processing UDP packets finished */

  }  /* end of forever loop */

exit_read_thread:

  g_rpc_thread_alive = 0;
  for(i = 0 ; i < num_thread_info ; i++) {  // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    NotifyMainThread(thread_info[i]); /* pgr0000 */  // LCOV_EXCL_LINE 6: double check
  }

  close(inotify_fd);

  if (normal_exit == 0) {  // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_CRIT("sub thread ABNORMALLY exited.");  // LCOV_EXCL_LINE 6: double check
  } else {
    RPC_LOG_DEBUG("sub thread normally exited.");
  }
  return NULL;
}

/* Notification of sub-thread -> main-thread (via pipe) */
RUNS_IN_READING_THREAD
  static void
NotifyMainThread(RpcThreadInfo *th) {
  rpc_assert(th->srvr_id->apicall != NULL);  // LCOV_EXCL_BR_LINE 6: double check
  char c = 0;
  write(th->srvr_id->apicall->pipe_sub_main[PIPE_WRITE], &c, sizeof(c));
}

/* Notification of main-thread -> sub-thread(via pipe) */
/* Termination instruction */
RUNS_IN_CALLERS_THREAD
  static void
KillRpcThread(void) {
  char buf[RPC_MAIN_SUB_COMMAND_SIZE];
  sprintf(buf, RPC_MAIN_SUB_COMMAND, RPC_COMMAND_EXIT, (unsigned long)0);
  write(g_rpc_pipe_main_sub[PIPE_WRITE], buf, sizeof(buf));
}

/* AddRPC_ID */
RUNS_IN_CALLERS_THREAD
  static void
NotifyAddServer(RpcThreadInfo *th) {
  char buf[RPC_MAIN_SUB_COMMAND_SIZE];
  sprintf(buf, RPC_MAIN_SUB_COMMAND, RPC_COMMAND_ADD_SERVER,
      (unsigned long)th);
  write(g_rpc_pipe_main_sub[PIPE_WRITE], buf, sizeof(buf));
}

/* Remove RPC_ID */
RUNS_IN_CALLERS_THREAD
  static void
NotifyRemoveServer(RpcThreadInfo *th) {
  char buf[RPC_MAIN_SUB_COMMAND_SIZE];
  sprintf(buf, RPC_MAIN_SUB_COMMAND, RPC_COMMAND_REMOVE_SERVER,
      (unsigned long)th);
  write(g_rpc_pipe_main_sub[PIPE_WRITE], buf, sizeof(buf));
}

/* Register the socket name of the source client in the management table. */
  static RPC_Result
RpcRegistSockName(RpcIdInfo *idinfo, char *client_sock_name, const struct ucred *cr, int wd) {
  if ((NULL == idinfo) || (NULL == client_sock_name) || (NULL == cr) || (0 > cr->pid)) {  // LCOV_EXCL_BR_LINE 6: void *RpcThreadMain(void *ptr)
    RPC_LOG_ERR("RpcRegistSockName() : Invalid Param.");  // LCOV_EXCL_START 6: void *RpcThreadMain(void *ptr)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RpcClientSockNameInfo *sock_name_buf, *current;

  sock_name_buf = rpc_malloc(sizeof(RpcClientSockNameInfo));
  // LCOV_EXCL_BR_START 5: fail safe for libc malloc
  if( sock_name_buf == NULL ){
    return RPC_ERR_Fatal;
  }
  // LCOV_EXCL_BR_STOP
  strcpy(sock_name_buf->client_sock_name, client_sock_name);  /* Socket name */
  sock_name_buf->pid = cr->pid;  /* PID */
  sock_name_buf->uid = cr->uid;  /* UID */
  sock_name_buf->gid = cr->gid;  /* GID */
  sock_name_buf->wd = wd;  /* Non-negative inotify monitored descriptors */
  sock_name_buf->next = NULL;  /* Pointer to next node (NULL since last node) */

  if (0 == RPC_client_sock_name_num(idinfo)) {
    RPC_sock_info_head(idinfo) = sock_name_buf;
  } else {
    for (current = RPC_sock_info_head(idinfo); current->next != NULL; current = current->next)
      ;
    current->next = sock_name_buf;
  }
  RPC_client_sock_name_num_inc(idinfo);
  return RPC_OK;
}

/* Check if the socket name of the source client is registered in the management table */
  static RPC_Result
RpcCheckSockName(RpcIdInfo *idinfo, RPC_ID client_id) {
  if ((NULL == idinfo) || (client_id == RPC_NO_ID)) {  // LCOV_EXCL_BR_LINE 6: void *RpcThreadMain(void *ptr)
    RPC_LOG_ERR("RpcCheckSockName() : Invalid Param.");  // LCOV_EXCL_START 6: void *RpcThreadMain(void *ptr)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  char buf[7], client_path_name[SOCK_NAME_LEN];  /* Client socket name */

  /* Converting client_id to the socket name associated with RPC_ID */
  RpcSetClientName(buf, client_id);
  memcpy(client_path_name, buf + 1, 5);
  client_path_name[5] = '\0';
  RpcClientSockNameInfo *current = RPC_sock_info_head(idinfo);

  /* Search source socket name in management table */
  while (NULL != current) {
    if (0 == strcmp(current->client_sock_name, client_path_name)) {  /* Registered socket name (authenticated) */
      return RPC_OK;
    }
    current = current->next;
  }
  return RPC_ERR_Fatal;  /* Not registerd socket name (unauthenticated) */
}

/* Remove source client socket name from management table */
  static RPC_Result
RpcDeleteSockName(RpcIdInfo *idinfo, int wd) {
  if ((NULL == idinfo) || (0 > wd)) {
    RPC_LOG_ERR("RpcDeleteSockName() : Invalid Param.");
    return RPC_ERR_Fatal;
  }

  RpcClientSockNameInfo *current, *previous;
  current = RPC_sock_info_head(idinfo);
  previous = current;
  int cnt = 0;

  /* Remove Source Socket Name in Management Table */
  while (NULL != current) {
    if (wd == current->wd) {  /* Delete element */
      if (0 == cnt) {  /* Delete the start element in the management table */
        RPC_sock_info_head(idinfo) = RPC_sock_info_head(idinfo)->next;
        rpc_free(current);
        current = RPC_sock_info_head(idinfo);
        cnt = -1;
      } else {  /* Delete other than the start element in the management table */
        previous->next = current->next;
        rpc_free(current);
        current = previous->next;
      }
      RPC_client_sock_name_num_dec(idinfo);
    } else {  /* Refer to the next node without deleting */
      previous = current;
      current = current->next;
    }
    cnt ++;
  }

  return RPC_OK;
}

/* Remove all source client socket names in the management table */
  static RPC_Result
RpcAllDeleteSockName(RpcIdInfo *idinfo, int inotify_fd) {
  if (NULL == idinfo) {  // LCOV_EXCL_BR_LINE 6: double check in void RpcDestroyThreadInfo(void)
    RPC_LOG_ERR("RpcAllDeleteSockName() : Invalid Param.");  // LCOV_EXCL_START 6: void RpcDestroyThreadInfo(void)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RpcClientSockNameInfo *current = RPC_sock_info_head(idinfo);

  while (NULL != current) {
    RpcClientSockNameInfo *previous = current;
    current = current->next;

    if (0 <= previous->wd) {
      inotify_rm_watch(inotify_fd, previous->wd);
    }

    rpc_free(previous);
    previous = NULL;
    RPC_client_sock_name_num_dec(idinfo);
  }

  return RPC_OK;
}


/* Check if client is allowed to communicate */
  static RPC_Result
RpcCheckClientCredential(RpcIdInfo *idinfo, const struct ucred *cr) {
  if ((NULL == idinfo) || (NULL == cr)) {  // LCOV_EXCL_BR_LINE 6: double check in void *RpcThreadMain(void *ptr)
    RPC_LOG_ERR("RpcCheckClientCredential() : Invalid Param.");  // LCOV_EXCL_START 6: void *RpcThreadMain(void *ptr)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  /* Retern RPC_OK if authentication is not required */
  if (NO_SECURE_CHECK == RPC_secure_check(idinfo)) {
    return RPC_OK;
  }

  INT32 i = 0;  /* Loop counter */

  /* Search client UID in registered UID list */
  for(i = 0; i < RPC_uid_num(idinfo); i++) {
    if(RPC_uid_list(idinfo, i) == cr->uid) {  /* Found UID in registered UID list */
      return RPC_OK;
    }
  }

  /* Search client GID in registered GID list */
  for(i = 0; i < RPC_gid_num(idinfo); i++) {
    if(RPC_gid_list(idinfo, i) == cr->gid) {  /* Found GID in registered GID list. */
      return RPC_OK;
    }
  }

  RPC_LOG_ERR("[Client isn't authenticated!!!!]");
  return RPC_ERR_Fatal;  /* Not found UID in registered UID list */
}
