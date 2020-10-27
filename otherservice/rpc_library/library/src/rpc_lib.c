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
 * @file rpc_lib.c
 * @brief RPC Library-API Function Implementation
 *
 */
/** @mainpage
 * This document describes the following API functions, structures, 
 * type declarations, and macros.
 * - RPC library external specification
 * - External specifications for RPC tools
 *
 * For an overview of other RPC libraries and tools and anomaly processing,
 * to reference the Word version of the documentation.
 * (external specifications and programming guides) */
/** @defgroup RPClib RPC library external specification */
/** @defgroup RPCtool RPC tool (rpc_apidef) */
/** @defgroup RPC_ID_assign RPC_ID Assignment */
/** @defgroup RPClib_in RPC library internal specifications (incomplete) */
/** @addtogroup RPClib
 * @{
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/inotify.h>

#include <other_service/rpc.h>
#include "rpc_internal.h"

#include <native_service/cl_process.h> // for CL_INTFY_FILENAME_FORMAT
#include <native_service/cl_monitor.h>


/* Prototype declarations */
static RPC_Result ProcessAPIcallRequest(RpcIdInfo *idinfo, RPC_ID client,
    UINT16 api_num, char *args_string,
    unsigned int args_size);
static RPC_Result RpcAllDeleteSrvrPid(RpcIdInfo *idinfo);
static RPC_Result RpcClientWaitCertification(RpcIdInfo *idinfo, pid_t *srvr_pid);
static RPC_Result RpcRegistSrvrPid(RpcIdInfo *idinfo, RPC_ID srvr_rpc_id, pid_t srvr_pid, int wd);

#define THREAD_INFO_CHECK(th) \
  if ((th) == NULL) { \
    RPC_LOG_STATE("No thread info"); \
    return RPC_ERR_Fatal; \
  }

#define SUB_THREAD_CHECK() \
  if (g_rpc_thread_alive == 0) { \
    RPC_LOG_STATE("No sub thread -- shutdown"); \
    RpcDestroyThreadInfo(); \
    return RPC_ERR_Fatal; \
  }

/**
 * Waiting for response packet for API call and API result transmission
 * Timeout time in msec.
 * If the other program is abnormal (process termination), delay occurs in the meanwhile.
 * Confirm the operation on the actual machine and set again.
 */
#define RESPONSE_WAIT_MSEC_DEFAULT 2000
static unsigned int Response_wait_msec = RESPONSE_WAIT_MSEC_DEFAULT;

char *RPC_DEBUG = NULL;

#if defined(RPC_USE_UNIX_AUTOBIND)

  static RPC_Result
CreateClientIdinfo(RpcThreadInfo *th, RPC_ID *pID) {
  if (RpcCreateIdInfo(th, *pID, NULL, NO_SECURE_CHECK) < 0) {
    RPC_LOG_ERR("Can't create id_info");
    return RPC_ERR_Fatal;
  }

  *pID = RPC_my_id(RPC_clnt_idinfo(th));
  RPC_LOG_DEBUG("RPC: assigned client ID %d", *pID);
  if (RPC_DEBUG) {  // LCOV_EXCL_BR_LINE 7: debug
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    printf("RPC[%s]: client start finished.\n", RPC_DEBUG);  // LCOV_EXCL_LINE 7: debug
  }
  return RPC_OK;
}
#else /* !AUTOBIND */

#include <sys/file.h>

  static int
FindUniqClientId(void) {
#define RPC_ID_FILE_NAME "/tmp/RPC/id"
#define RPC_MIN_CLIENT_ID 1024
#define RPC_MAX_CLIENT_ID (1024+32768-1)

  int fd, found;
  RPC_ID id, next;
  struct stat statbuf;
  char fname[32];

  fd = open(RPC_ID_FILE_NAME, O_RDWR|O_CLOEXEC, 0777);
  if (fd < 0) {
    id = RPC_MIN_CLIENT_ID;
    fd = open(RPC_ID_FILE_NAME, O_RDWR|O_CREAT|O_CLOEXEC, 0777);
    if (fd < 0) {
      return -1;
    }
    flock(fd, LOCK_EX); // Possible sleep here
  } else {
    flock(fd, LOCK_EX); // Possible sleep here
    read(fd, &id, sizeof(id));
  }

  found = 0;
  while(found == 0) {
    rpc_set_socket_name(fname, id);
    if (stat(fname, &statbuf) < 0) {
      found = 1;
      break;
    }
    id++;
    if (id > RPC_MAX_CLIENT_ID) {
      id = RPC_MIN_CLIENT_ID;
    }
  }

  next = id + 1;
  if (next > RPC_MAX_CLIENT_ID) {
    next = RPC_MIN_CLIENT_ID;
  }

  lseek(fd, 0, SEEK_SET);
  write(fd, &next, sizeof(next));
  flock(fd, LOCK_UN);
  close(fd);

  return id;
}

  static RPC_Result
CreateClientIdinfo(RpcThreadInfo *th, UINT16 port) {
  if (RpcCreateIdInfo(th, port, NULL, NO_SECURE_CHECK) < 0) {
    RPC_LOG_ERR("Can't create id_info");
    return RPC_ERR_Fatal;
  }
  // LCOV_EXCL_BR_START 7: debug
  if (RPC_DEBUG) {
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    printf("RPC[%s]: client start finished.\n", RPC_DEBUG);
  }
  // LCOV_EXCL_BR_STOP
  return RPC_OK;
}
#endif /* AUTOBIND */

/** @fn RPC_START_SERVER(ID)
 * @brief RPC_start() macro for the server
 *
 * Used by RPC-API server programs to activate the RPC library.
 * ID specifies a symbol of the form XXX_RPC_ID defined in rpc_id.h.
 * Only one server program per thread is allowed.
 * When a server program calls an API as a client of another server, 
 * it only needs to call RPC_START_SERVER, 
 * not RPC_START_CLIENT.
 *
 * RPC_end() should be called when the RPC library is no longer needed.
 * @param ID [IN]RPC_ID of the program
 * @see RPC_start
 * @see RPC_end
 */
/** @fn RPC_START_CLIENT(pID)
 * @brief RPC_start() macro for clients
 *
 * Use only the client functions of the RPC library
 * (only calling APIs provided by other programs),
 * This macro is used for the start process.
 *
 *
 * RPC_end() should be called when the RPC library is no longer needed.
 *
 * @param pID [OUT]Pointer to the RPC_ID of the program
 * @see RPC_start
 * @see RPC_end
 */
/** @brief Start processing of the RPC library
 *
 * - With an open socket, RPC_ID, and API call dispatching functions
 *   Register in the global variable (managed separately for each thread and RPC_ID)
 * - Starting use as a server
 *   - func specifies a function pointer (defined in stub) that is not a NULL.
 *   - Specifies a fixed ID defined in rpc_id.h as the ID.
 *   - If the same thread has already started using the server, an error code is returned.
 *   - When the server function is started for the first time in a process, 
 *     one subthread is started to process received packets 
 *     in the background.
 * - Starting use as a client
 *   - NULL is specified in the func.
 *   - In principle, an invalid ID value (RPC_NO_PORT) is specified.
 *     This function automatically assigns IDs and returns the assignment result to pID.
 *     - However, if another program has already started using it as a client, 
 *       it shares the socket/RPC_ID instead of creating a new socket.
 *   - Exceptionally, it is allowed to specify a fixed ID defined in rpc_id.h as the ID.
 *     This is the only way to use RPC_start() in programs 
 *     where it is difficult to always call RPC_end() at the end of use by program structure/design.
 *     - In this case, the pID argument is not referenced.
 * - The maximum number of threads that use the RPC library in a process is 
 *   specified by RPC_MAX_THREADS_IN_PROCESS.
 *
 * @param id [IN]RPC_ID of the program (in the case of RPC_NO_PORT, this means automatic assignment)
 * @param dispatch_func Dispatching functions (defined in stub files for servers, 
 * and NULL specified in programs that do not provide APIs)
 * @param pID [OUT]RPC_ID of the automatically assigned program
 *
 * @retval RPC_OK: normal completion
 * @retval Otherwise: Abnormal termination (FATAL)
 *
 * @note Normally, this function is not called directly, but the following macro is used.
 * - RPC_START_SERVER(ID)
 *   For programs that provide RPC server functions.
 *   This macro is also used when both server and client functions are used.
 * - RPC_START_CLIENT(pID)
 *   For programs that use only the RPC client function.
 */
  RPC_Result
RPC_start(RPC_ID id, RPC_dispatch_func_t func, RPC_ID *pID, INT32 secure_check)
{
#if defined(RPC_USE_SYSLOG)
  static int init_syslog = 0;

  if (init_syslog == 0) {
    openlog("librpc", LOG_PID, LOG_USER);
    setlogmask(LOG_UPTO(LOG_INFO));
    /* setlogmask(LOG_MASK(LOG_DEBUG)); */
    init_syslog = 1;
    if (func != NULL)
      RPC_LOG_STATE("RPC_START_SERVER(%#x)\n", id);
  }
#endif /* RPC_USE_SYSLOG */

  if (func != NULL && id == RPC_NO_ID) { /* No ID is specified despite as a server */
    RPC_LOG_ERR("Invalid RPC_start server arg");
    return RPC_ERR_Fatal;
  }
  if (func != NULL && pID != NULL) { /* Requested obtaining pID despite as a server */  // LCOV_EXCL_BR_LINE 200: macro guarantee impossible both not NULL, RPC_START_***
    RPC_LOG_ERR("Invalid RPC_start server arg");  // LCOV_EXCL_START 200: macro guarantee impossible both not NULL, RPC_START_***
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP
  if (func == NULL && id != RPC_NO_ID) { /* Specified ID despite as a client */
    RPC_LOG_ERR("Invalid RPC_start server arg");
    return RPC_ERR_Fatal;
  }
#if defined(RPC_USE_UNIX_AUTOBIND)
  if (func == NULL && pID == NULL) {
    /* pID is NULL despite as a client (not supported for ID-specified clients) */
    RPC_LOG_ERR_W_NAME("RPC_Start(ID, NULL, NULL) is no longer supported, ID is auto-assigned");
    return RPC_ERR_Fatal;
  }
#else /* !AUTOBIND */
  if (func == NULL && id == RPC_NO_ID && pID == NULL) {
    /* pID is NULL despite as an auto-assigned client */
    RPC_LOG_ERR("Invalid RPC_start arg");
    return RPC_ERR_Fatal;
  }
#endif /* !AUTOBIND */
  if ((secure_check != NO_SECURE_CHECK) && (secure_check != NEED_SECURE_CHECK)) {  // LCOV_EXCL_BR_LINE 200: macro guarantee only two value, RPC_START_***
  /* Check Arguments for Client Authentication */
    RPC_LOG_ERR("Invalid RPC_start arg");  // LCOV_EXCL_START 200: macro guarantee only two value, RPC_START_***
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RpcThreadInfo *th = RpcMyThreadInfo();
  if (th == NULL) {
    th = RpcCreateThreadInfo();
    if (th == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
      return RPC_ERR_Fatal;
    }
  }

  /* RPC_start is already done in the same thread */
  if (func != NULL) {/* Server start */
    if (RPC_srvr_idinfo(th) != NULL) {/* Server already started */
      RPC_LOG_ERR("duplicated RPC_START_SERVER");
      return RPC_ERR_Fatal;
    }
  } else { /* Client start */
    if (RPC_clnt_idinfo(th) != NULL) {/* Client already started */
      if (pID != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
        *pID = (RPC_clnt_idinfo(th))->port;
        RPC_LOG_DEBUG("use existing client ID=%d", *pID);
        RPC_inc_clnt_count(RPC_clnt_idinfo(th));
      }
      /*
       * When pID == NULL (RPC_ID fixed), there is no guarantee 
       * that RPC_end() will be called as many times as needed, so it does not count up.
       */
      return RPC_OK;
    }
  }

  if (func == NULL) {/* Client start */
#if defined(RPC_USE_UNIX_AUTOBIND)
    /* Automatically assign everything without supporting ID specification */
    RPC_ID id_local = RPC_NO_ID;
    RPC_Result ret = CreateClientIdinfo(th, &id_local);
    if(RPC_OK != ret) return ret;

    if (pID) {  // LCOV_EXCL_BR_LINE 6: double check
      *pID = id_local;
    }
    RPC_clnt_inotify_fd(RPC_clnt_idinfo(th)) = inotify_init1(IN_CLOEXEC);
    return ret;
#else /* !AUTOBIND */
    if (id == RPC_NO_ID) {
      /* No port number specified -> Automatically assign an ID and port number here */
      /*
       * FindUniqClientId() returns ID within the range of RPC_MIN_CLIENT_ID(1024) to RPC_MAX_CLIENT_ID(1024+32768-1)
       */
      int ret;
      ret = FindUniqClientId();
      if (ret < 0) {
        return RPC_ERR_Fatal;
      }
      /* pID != NULL is already checked */
      *pID = (UINT16)ret;
      return CreateClientIdinfo(th, *pID);
    } else {
      /*
       * Client start with specified port number
       * Programs that cannot guarantee an RPC_end() call 
       * at termination specify a fixed RPC_ID from the upper level and use it.
       * (To avoid memory shortage caused by building up socket-files in /tmp/RPC)
       */
      return CreateClientIdinfo(th, id);
    }
#endif /* !AUTOBIND */
  }

  /* Dispatch func specified (servers) -> bind */
  CL_MonitorInit(CL_MONITOR_INIT_USER);    /* Using the API for Error Monitoring */

  if (RpcCreateIdInfo(th, id, func, secure_check) < 0) {
    RPC_LOG_ERR("Can't create id_info");
    return RPC_ERR_Fatal;
  }

  if (RPC_DEBUG) {  // LCOV_EXCL_BR_LINE 7: debug
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    printf("RPC[%s]: server thread invoked\n", RPC_DEBUG);  // LCOV_EXCL_LINE 7: debug
  }
  char *env;
  if ((env = getenv("RPC_RESPONSE_WAIT")) != NULL) {
    Response_wait_msec =  (unsigned int)strtol(env, NULL, 10);
  }

  return RPC_OK;
}

  static void
RpcEndSub(RpcThreadInfo *th, RpcIdInfo *idinfo) {
  int sockfd;
#if !defined(RPC_USE_UNIX_AUTOBIND)
  char path[120];

  rpc_set_socket_name(path, RPC_my_id(idinfo));
#endif /* !AUTOBIND */
  sockfd = idinfo->sock;
  RpcDestroyIdInfo(th, idinfo);
  close(sockfd);
#if !defined(RPC_USE_UNIX_AUTOBIND)
  unlink(path);
#endif /* !AUTOBIND */
}

/** @brief RPC Library End-of-Use Processing
 *
 * - Perform termination processing such as closing the opened socket, releasing the memory.
 *   (Server/Client Common)
 * - If there are no programs in the thread that use RPC_ID, 
 *   the subthread started by RPC_start is terminated.
 * - Each RPC_ID must be called before the program exits.
 * - RPC_end must be called by the thread that performed the RPC_start.
 * - Ignored unused or no longer used RPC_ID if specified.
 *
 * @param ID [IN]RPC_ID of the program
 */
  void
RPC_end(RPC_ID id) {
  RpcThreadInfo *th = RpcMyThreadInfo();
  if (th == NULL) {
    return;
  }
  RpcIdInfo *idinfo;
  idinfo = RPC_srvr_idinfo(th);
  if (idinfo != NULL && RPC_my_id(idinfo) == id) {/* srvr */
    RpcEndSub(th, idinfo);
  }

  idinfo = RPC_clnt_idinfo(th);
  if (idinfo != NULL && RPC_my_id(idinfo) == id) {/* client */

    RPC_dec_clnt_count(idinfo);
    if (RPC_clnt_count(idinfo) == 0) {
    /* Deleting management table of the communication destination server PID */
    RpcAllDeleteSrvrPid(idinfo);
    close(RPC_clnt_inotify_fd(idinfo));

      RpcEndSub(th, idinfo);
    }
  }
  if (RPC_srvr_idinfo(th) == NULL && RPC_clnt_idinfo(th) == NULL) {
    RPC_LOG_DEBUG("destroy thread info %p", th);
    RpcDestroyThreadInfo();
  }
}

/** @brief RPC library use termination processing (forced cleanup at process termination)
 *
 * Deletes the socket file created for RPC communication in the process.
 * For  to release the resources of programs that use the RPC library with RPC_end() in principle, 
 * but this API is an insurance process to eliminate socket file leaks.
 *
 * - Since it is assumed that the socket is called immediately before the process terminates, 
 *   closing sockets and releasing memory is not performed automatically when the process terminates.
 * - All RPC processing (both server and client) after this API call cannot be used.
 *   Operation is not guaranteed even if RPC_START is performed again after that.
 *
 * No action for versions of the RPC library that use Unix autobind.
 */
  void
RPC_end_all(void) {
#if !defined(RPC_USE_UNIX_AUTOBIND)
  RpcUnlinkSocketFiles();
#endif
}

/**
 * @fn RPC_Result RPC_set_API_timeout(RPC_ID id, UINT32 sec)
 * @brief Setting the timeout period during server API processing
 *
 * @param sec [IN] Timeout (sec) Set within the range of 1 to 60 sec.
 *
 * @retval RPC_OK: normal completion
 * @retval RPC_ERR_Configuration: Argument out of range
 * @retval RPC_ERR_Fatal: Fatal error
 */
  RPC_Result
RPC_set_API_timeout(INT32 sec) {
  /* param check */
  if ((sec > 60) || (sec < 1)) {
    RPC_LOG_ERR( "RPC_set_API_timeout() : Invalid Param." );
    return RPC_ERR_Configuration;
  }

  /* thread_info check */
  RpcThreadInfo *th = RpcMyThreadInfo();
  THREAD_INFO_CHECK(th);

  /* thread_info->id_info check */
  if (RPC_srvr_idinfo(th) != NULL ) {  // LCOV_EXCL_BR_LINE 200: server start abnormal can't test
    RpcIdInfo *idinfo = RPC_srvr_idinfo(th);

    /* thread_info->id_info->apicall_info check */
    if( RPC_apicall_info(idinfo) != NULL ) {  // LCOV_EXCL_BR_LINE 200: server start abnormal can't test
      RPC_apicall_api_timeout_sec(idinfo) = sec;
    } else {  // LCOV_EXCL_START 200: server start abnormal can't test
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      RPC_LOG_ERR( "No apicall_info" );
      return RPC_ERR_Fatal;
    }

  } else {
    RPC_LOG_STATE("No srvr_id info");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP
  return RPC_OK;
}


/** @brief API call execution function (client side)
 *
 * (These functions are called from within a client-facing STUB. Users never use these directly.)
 */
  RPC_Result
RPC_API_call(RPC_ID server, UINT16 api_num,
    const char *args_string, unsigned int args_size,
    char **ret_string, unsigned int *ret_size) {
  *ret_string = NULL;

  RpcThreadInfo *th = RpcMyThreadInfo();

  THREAD_INFO_CHECK(th);

  RpcIdInfo *idinfo;
  idinfo = RPC_clnt_idinfo(th);

  pid_t srvr_pid;

  if (idinfo == NULL) {  // LCOV_EXCL_BR_LINE 8: dead code, idinfo always isn't NULL
    /*
     * In the past, the remedy of uninitialized clients has been done here.
     * However, because it becomes to accumulate garbage in the /tmp/RPC by changing UNIX sockets,
     * as an error without remedies.
     */
    // LCOV_EXCL_START 8: dead code, idinfo always isn't NULL
    RPC_LOG_ERR_W_NAME("warning!!! RPC call without RPC_start(): server=%d, api_num=%d", server, api_num);
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
#if defined(RPC_USE_UNIX_AUTOBIND)
    RPC_ID id = RPC_NO_ID;
    RPC_Result ret = CreateClientIdinfo(th, &id);
#else /* !AUTOBIND */
    int id;
    id = FindUniqClientId();
    if (id < 0) {
      return RPC_ERR_Fatal;
    }
    RPC_Result ret = CreateClientIdinfo(th, (UINT16)id);
#endif /* !AUTOBIND */
    if (ret != RPC_OK) {
      return ret;
    }
    idinfo = RPC_clnt_idinfo(th);
    if (idinfo == NULL) {
      return RPC_ERR_Fatal;
    }
  }  // LCOV_EXCL_STOP

  char api_head[RPC_APICALL_FORMAT_ARGS_START + 1];
  sprintf(api_head, RPC_APICALL_FORMAT, api_num);

  rpc_send_buf sendbuf[2];
  sendbuf[0].buf = api_head;
  sendbuf[0].bytes = RPC_APICALL_FORMAT_ARGS_START;
  if (args_size > 0) {
    sendbuf[1].buf = args_string;
    sendbuf[1].bytes = args_size;
  }

  int seq_num;

send_api_call:
  seq_num = RpcSendUdp2(idinfo, server,
      (api_num == RPC_API_NUM_RPC_ALIVE
       ? RPC_SEND_TO_SERVER_NO_RETRY
       : RPC_SEND_TO_SERVER),
      RPC_PACKET_APICALL,
      (args_size > 0) ? 2 : 1, sendbuf);
  if (seq_num < 0) {
    return RPC_ERR_No_Response;
  }

  // wait for response
  UINT16 response;
  RPC_Result result = RpcClientWaitResponse(idinfo, (UINT32)seq_num, Response_wait_msec,
      &response);
  if (result != RPC_OK) {
    return result;
  }

  if (response == RPC_RESPONSE_API_BUSY) {        /* Server is busy */
    return RPC_ERR_Busy;
  } else if (response == RPC_RESPONSE_API_ERR) {    /*  API call error */
    RPC_LOG_STATE("APIcall error (can't queue)");
    return RPC_ERR_Fatal;
  } else if (response == RPC_RESPONSE_API_DEADLOCK) {  /* Server is in deadlock */
    RPC_LOG_STATE("server DeadLock.");
    return RPC_ERR_Server_DeadLock;
  } else if (response == RPC_RESPONSE_API_CERTIFY) {  /* Authentication request */
    struct sockaddr_un srvr_sa_un, secure_sa_un;
    socklen_t srvr_sa_len, secure_sa_len;
    int secure_sock_un;

    /* Create client socket for authentication */
    memset(&secure_sa_un, 0, sizeof(secure_sa_un));
    secure_sa_un.sun_family = AF_UNIX;
    secure_sock_un = socket(PF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
    RpcSetClientSecureName(secure_sa_un.sun_path, RPC_port(idinfo));
    secure_sa_len = (socklen_t)(sizeof(secure_sa_un.sun_family) + SECURE_SOCK_NAME_LEN);
    if (bind(secure_sock_un, (struct sockaddr *)&secure_sa_un, secure_sa_len) < 0) {
      RPC_LOG_PERROR("STREAM : bind(unix), ID:%#x", RPC_port(idinfo));
    }
    idinfo->secure_sock = secure_sock_un;

    /* Destination Server Socket Setting */
    memset(&srvr_sa_un, 0, sizeof(srvr_sa_un));
    srvr_sa_un.sun_family = AF_UNIX;
    RpcSetServerSecureName(srvr_sa_un.sun_path, server);
    srvr_sa_len = (socklen_t)(sizeof(srvr_sa_un.sun_family) + SECURE_SOCK_NAME_LEN);

    /* Connection */
    int cr = connect(RPC_my_secure_sock(idinfo), (struct sockaddr *)&srvr_sa_un, srvr_sa_len);
    if (0 > cr) {
      RPC_LOG_STATE("client connect() is failed.");
      return RPC_ERR_Fatal;
    }

    /* Waiting for server authentication */
    RPC_Result ret_certify = RpcClientWaitCertification(idinfo, &srvr_pid);
    close(RPC_my_secure_sock(idinfo));
    if (RPC_OK != ret_certify) {/* Authentication NG */
      RPC_LOG_STATE("denied the connection.");
      return ret_certify;
    }

    /* inotify monitoring of server processes */
    char intfy_fname[32];
    snprintf(intfy_fname, sizeof(intfy_fname), CL_INTFY_FILENAME_FORMAT, srvr_pid);
    int wd = inotify_add_watch(RPC_clnt_inotify_fd(idinfo), intfy_fname, IN_DELETE_SELF);
    if (0 > wd) {
      RPC_LOG_STATE("intfy_fname is Not Found [%s].", intfy_fname);
    }

    /* Register the communication destination server PID in the management table. */
    if (RPC_OK != RpcRegistSrvrPid(idinfo, server, srvr_pid, wd)) {
      return RPC_ERR_Fatal;
    }
    goto send_api_call;  /* Sending the API request again */

  } else if (response == RPC_RESPONSE_NONE) {  // LCOV_EXCL_BR_LINE 8: dead code, not implement
    // Not implemented to cancel processing
    // LCOV_EXCL_START 8: dead code, not implement
    RPC_LOG_STATE("No response for API call");
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_No_Response;
  }  // LCOV_EXCL_STOP

  // When the server ALIVE is queried, no result is returned, so return here
  if (api_num == RPC_API_NUM_RPC_ALIVE) {
    return RPC_OK;
  }

  // API call is accepted at server
  // now wait for the return packet
  result = RpcClientWaitResult(idinfo, server);
  if (result != RPC_OK) {
    RpcDiscardAPIcallReturn(idinfo);
    return result;
  }

  /****** Function error and return value must be handled separately *******/
  UINT8 *return_str = RPC_apicall_return_str(idinfo); /* pgr0689 */
#ifdef APICALL_RETURN_TIMEOUT
  if (return_str  == NULL) {
    RPC_LOG_STATE("APIcall return timeout");
    return RPC_ERR_No_Response;
  }
#else
  rpc_assert(return_str != NULL);  // LCOV_EXCL_BR_LINE 6: double check
#endif

  RPC_THREAD_MUTEX_LOCK(idinfo->thread_info);

  *ret_string = rpc_malloc(RPC_apicall_return_str_len(idinfo));
  if (*ret_string != NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    if (ret_size) {
      *ret_size = RPC_apicall_return_str_len(idinfo);
    }
    memcpy(*ret_string, RPC_apicall_return_str(idinfo),
        RPC_apicall_return_str_len(idinfo));
  } else {
    RPC_LOG_STATE("APIcall: No Memory");  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    result = RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP
  RPC_THREAD_MUTEX_UNLOCK(idinfo->thread_info);
  RpcDiscardAPIcallReturn(idinfo);

  return result;
}

/** @brief Releasing allocated memory while API call is processing
 *
 * (These functions are called from within a client-facing STUB. Users never use it directly.)
 */
  void
RPC_free_return_string(void *ptr) {
  if (ptr != NULL) {
    rpc_free(ptr);
  }
}

/** @brief Retrieving file descriptors for determining whether RPC-API calls are requested
 *
 * - This function returns a file descriptor used to determine whether or not there is a request to the server.
 * - For the FD returned by this function, Don't use the other way than the FD specified 
 *   in the poll or select parameter.
 * - Intended use
 *   - Obtain FD using this function.
 *   - Wait for data to arrive by poll or select.
 *   - Call RPC_process_API_request() when data arrived.
 *   - This procedure performs handling request of the API call from the client.
 *     (Server-side API function is executed.)
 *
 * @param id [IN]RPC_ID of the server program.
 * @param fd [OUT]Storage of file descriptor
 *
 * @retval RPC_OK: normal completion
 * @retval Otherwise: Abnormal termination (FATAL)
 * @see RPC_process_API_request
 */
  RPC_Result
RPC_get_fd(RPC_ID id, int *fd) {
  if ((id == RPC_NO_ID) || (fd == NULL)) {
    return RPC_ERR_Fatal;
  }
  rpc_assert(fd != NULL);  // LCOV_EXCL_BR_LINE 6: double check

  RpcThreadInfo *th = RpcMyThreadInfo();

  THREAD_INFO_CHECK(th);

  SUB_THREAD_CHECK();  // LCOV_EXCL_BR_LINE 15: macro define in rpc_lib.c

  RpcIdInfo *idinfo = RPC_srvr_idinfo(th);

  rpc_assert(idinfo != NULL && RPC_my_id(idinfo) == id);  // LCOV_EXCL_BR_LINE 6: double check

  *fd = RPC_pipe_sub_main(th)[PIPE_READ];

  return RPC_OK;
}

/** API Call Request Handling
*/
  static RPC_Result
ProcessAPIcallRequest(RpcIdInfo *idinfo, RPC_ID client, UINT16 api_num,
    char *args_string, unsigned int args_size) {
  /* ----------------------------
   * call API dispatch function
   * ----------------------------
   */

  /* API Running Client RPC_ID Setting */
  RPC_apicall_in_process_client(idinfo) = client;

  char *ret_string = NULL;
  unsigned int ret_size = 0;
  RPC_dispatch_func_t dispatch = RPC_apicall_dispatch_func(idinfo);
  rpc_assert(dispatch != NULL);  // LCOV_EXCL_BR_LINE 6: double check
  RPC_Result api_ret;
  api_ret = dispatch(api_num, args_string, args_size, &ret_string, &ret_size); /* pgr0060 */

  unsigned char retcode[RPC_RETCODE_LEN+1];
  sprintf((char *)retcode, "%08x ", api_ret);

  rpc_send_buf sendbuf[2];
  sendbuf[0].buf = retcode;
  sendbuf[0].bytes = sizeof(retcode) - 1;

  int seq_num;

  if (ret_string != NULL) {
    sendbuf[1].buf = ret_string;
    sendbuf[1].bytes = ret_size;
    seq_num = RpcSendUdp2(idinfo, client, RPC_SEND_TO_CLIENT,
        RPC_PACKET_APIRETURN, 2, sendbuf);
  } else {
    seq_num = RpcSendUdp2(idinfo, client, RPC_SEND_TO_CLIENT,
        RPC_PACKET_APIRETURN, 1, sendbuf);
  }
  RPC_marshall_free(1, ret_string);

  if (seq_num < 0) {
    RPC_LOG_ERR("send result failed!");
    return RPC_ERR_Fatal;
  }

  /* Set API Running Client RPC_ID to None */
  RPC_apicall_in_process_client(idinfo) = RPC_NO_PORT;

  /* Sleep API-processing entries */
  RPC_THREAD_MUTEX_LOCK(idinfo->thread_info);
  CL_MonitorSetEntry(CL_MONITOR_TYPE_RPC, RPC_port(idinfo),
      CL_MONITOR_STATE_SLEEP, 0, 0);
  RPC_THREAD_MUTEX_UNLOCK(idinfo->thread_info);

  return api_ret;
}

/** @brief Executing RPC-API calls
 *
 * Process all queued API execution requests from clients.
 * The API function of the server program is called from here.
 * If there is no API execution request, the system returns normally immediately without waiting.
 *
 * @param id [IN]RPC_ID of the server program
 * @retval RPC_OK: normal completion
 * @retval Otherwise: Fatal errors
 */
  RPC_Result
RPC_process_API_request(RPC_ID id) {
  if (id == RPC_NO_ID) {
    return RPC_ERR_Fatal;
  }
  RpcThreadInfo *th = RpcMyThreadInfo();

  THREAD_INFO_CHECK(th);

  SUB_THREAD_CHECK();

  RpcIdInfo *idinfo = RPC_srvr_idinfo(th);

  rpc_assert(idinfo != NULL && id == RPC_my_id(idinfo));  // LCOV_EXCL_BR_LINE 6: double check

  rpc_assert(RPC_apicall_info(idinfo) != NULL /* pgr0689 */   // LCOV_EXCL_BR_LINE 6: double check
      && RPC_apicall_dispatch_func(idinfo) != NULL);  // LCOV_EXCL_BR_LINE 6: double check

  char c;
  read(RPC_pipe_sub_main(th)[PIPE_READ], &c, sizeof(c));

  /* Process all API calls here */
  UINT16 api_num;
  RPC_ID client = RPC_NO_PORT;
  RPC_Result result = RPC_OK;
  char *args_string;
  unsigned int args_size;
  do {
    api_num = RpcGetAPIRequest(idinfo, &client,
        &args_string, &args_size);

    if (api_num > 0) {/* API calls were queued */
      result = ProcessAPIcallRequest(idinfo, client, api_num,
          args_string, args_size);

      RpcFreeAPIArgsString(args_string);

      if (result == RPC_ERR_API_Fatal || result == RPC_ERR_Fatal) {
        break;
      } else {
        result = RPC_OK;
      }
    }
  } while(api_num > 0);

  return result;
}

/** @brief Returns the operational state of the RPC server
 *
 * Returns whether or not the server with the specified RPC_ID is running.
 *
 * @param id [IN]RPC_ID of the server program
 * @retval RPC_OK: Running
 * @retval RPC_ERR_No_Response: Server is not running
 */
  RPC_Result
RPC_is_server_ready(RPC_ID id) {
#if defined(RPC_USE_UNIX_AUTOBIND)
  char *ret_string = NULL;
  unsigned int ret_size;
  RPC_Result ret;

  // Invoke Internal-API Calls for Server ALIVE Queries
  ret = RPC_API_call(id, RPC_API_NUM_RPC_ALIVE, NULL, 0,
      &ret_string, &ret_size);
  RPC_free_return_string(ret_string);

  return ret;
#else /* !AUTOBIND */
  struct stat statbuf;
  int err;
  char sockname[128];

  /* Check if server socket is created */
  rpc_set_socket_name(sockname, id);
  err = stat(sockname, &statbuf);
  if (err == 0) {
    return RPC_OK;
  } else {
    return RPC_ERR_No_Response;
  }
#endif /* !AUTOBIND */
}

/** @brief Registering UIDs and GID Lists
 *
 * Register a list of UIDs and GIDs that can be accepted by the RPC server.
 * A function used by the RPC server after RPC_START_SECURE_SERVER().
 * Return error if called from an RPC client or an RPC server that has not called RPC_START_SECURE_SERVER().
 *
 * @param uid_num          [IN] Number of registered UIDs (number of elements in UID list array)
 * @param *uid_list          [IN] Start pointer of registered UID list
 * @param gid_num          [IN] Number of registered GIDs (number of elements in the array of GID lists)
 * @param *gid_list          [IN] Start pointer of registered GID list
 * @retval RPC_OK:              List registration completed
 * @retval RPC_ERR_Fatal:       Failed to save to list
 */
RPC_Result
RPC_regist_credential(int uid_num, uid_t *uid_list, int gid_num, gid_t *gid_list) {
  RpcThreadInfo *th = RpcMyThreadInfo();
  THREAD_INFO_CHECK(th);

  if (NULL == RPC_srvr_idinfo(th)) {
    RPC_LOG_ERR("No server thread info");
    return RPC_ERR_Fatal;
  }

  RpcIdInfo *idinfo = RPC_srvr_idinfo(th);
  if ((NULL == idinfo) || (NULL == RPC_apicall_info(idinfo))) {  // LCOV_EXCL_BR_LINE 8: dead code, idinfo always isn't NULL
    // LCOV_EXCL_START 8: dead code, idinfo always isn't NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_ERR("No RPC_ID info");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  /* Neither UID nor GID list is specified. */
  if ((NULL == uid_list) && (NULL == gid_list)) {
    RPC_LOG_ERR("RPC_regist_credential() : UID and GID list is NULL.");
    return RPC_ERR_Fatal;
  }

  /* UID list argument check */
  if ((NULL == uid_list) && ((0 < uid_num ) && (uid_num <= CREDENTIAL_LIST_NUM_MAX))) {/* UID list is invalid */
    RPC_LOG_ERR("RPC_regist_credential() : UID list is illegal.");
    return RPC_ERR_Fatal;
  } else if ((NULL != uid_list) && ((1 > uid_num) || (uid_num > CREDENTIAL_LIST_NUM_MAX))) {/* Number of UID list elements is invalid */
    RPC_LOG_ERR("RPC_regist_credential() : UID list num is illegal.");
    return RPC_ERR_Configuration;
  }

  /* GID list argument check */
  if ((NULL == gid_list) && ((0 < gid_num ) && (gid_num <= CREDENTIAL_LIST_NUM_MAX))) {/* GID list is invalid */
    RPC_LOG_ERR("RPC_regist_credential() : GID list is illegal.");
    return RPC_ERR_Fatal;
  } else if ((NULL != gid_list) && ((1 > gid_num) || (gid_num > CREDENTIAL_LIST_NUM_MAX))) {/* Number of GID list elements is invalid */
    RPC_LOG_ERR("RPC_regist_credential() : GID list num is illegal.");
    return RPC_ERR_Configuration;
  }

  if (NO_SECURE_CHECK == RPC_secure_check(idinfo)) {/* Return error if registration is not necessary */
    RPC_LOG_ERR("Needless secure check.");
    return RPC_ERR_Fatal;
  }

  if (REGISTERED == RPC_regist_credential_info(idinfo)) {/* Return error if a list has already been registered */
    RPC_LOG_ERR("credential info has been registered already.");
    return RPC_ERR_Fatal;
  }

  /* Allocate as many memory areas as there are UID and GID lists */
  RPC_apicall_info(idinfo)->uid_list = rpc_malloc(sizeof(uid_t) * (unsigned int)uid_num);
  if (NULL == RPC_apicall_info(idinfo)->uid_list) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    RPC_LOG_ERR("Can't malloc uid_list.");  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP
  RPC_apicall_info(idinfo)->gid_list = rpc_malloc(sizeof(gid_t) * (unsigned int)gid_num);
  if (NULL == RPC_apicall_info(idinfo)->gid_list) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
    RPC_LOG_ERR("Can't malloc gid_list.");  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  /* Copy UID and GID registration list and number of elements */
  memcpy(RPC_apicall_info(idinfo)->uid_list, uid_list, sizeof(uid_t) * (unsigned int)uid_num);    /* UID list */
  memcpy(RPC_apicall_info(idinfo)->gid_list, gid_list, sizeof(gid_t) * (unsigned int)gid_num);    /* GID list */
  RPC_uid_num(idinfo) = uid_num;    /* Number of elements in the UID list */
  RPC_gid_num(idinfo) = gid_num;    /* Number of elements in the GID list */

  /* Set whether to register authentication information to Yes/No */
  RPC_regist_credential_info(idinfo) = REGISTERED;

  return RPC_OK;
}

/** @brief Obtain authentication info (UID, GID) of API requesting client
 *
 * Obtain the UID and GID of the client that requested the API processing being executed.
 * Return error if called from an RPC client or to an RPC server that has not called RPC_START_SECURE_SERVER().
 * This function should be called within an API function (in other words, API is being executed) provided to the client.
 * if called the other function, error is returned because there is no API being executed.
 *
 * @param *client_uid          [OUT] Requesting client UID of running API
 * @param *client_gid          [OUT] Requesting client GID of running API
 * @retval RPC_OK:              Succeeded to obtain of authentication info
 * @retval RPC_ERR_Fatal:       Failed to obtain of authentication Info
 */
RPC_Result
RPC_get_client_credential(uid_t *client_uid, gid_t *client_gid) {
  RpcThreadInfo *th = RpcMyThreadInfo();
  THREAD_INFO_CHECK(th);

  if (NULL == RPC_srvr_idinfo(th)) {
    RPC_LOG_ERR("No server thread info");
    return RPC_ERR_Fatal;
  }

  RpcIdInfo *idinfo = RPC_srvr_idinfo(th);
  if ((NULL == idinfo) || (NULL == RPC_apicall_info(idinfo))) {  // LCOV_EXCL_BR_LINE 8: dead code, idinfo always isn't NULL
    // LCOV_EXCL_START 8: dead code, idinfo always isn't NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_ERR("No RPC_ID info");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  if ((NULL == client_uid) || (NULL == client_gid)) {/* Argument check */
    RPC_LOG_ERR("RPC_regist_credential() : Invalid Param.");
    return RPC_ERR_Fatal;
  }

  if (NO_SECURE_CHECK == RPC_secure_check(idinfo)) {/* Return error if no security */
    RPC_LOG_ERR("Needless secure check.");
    return RPC_ERR_Fatal;
  }

  if (RPC_NO_PORT == RPC_apicall_in_process_client(idinfo)) {/* Return error if the API is not being executed */
    RPC_LOG_ERR("NOT in process APIcall.");
    return RPC_ERR_Fatal;
  }

  char buf[7], client_path_name[SOCK_NAME_LEN];  /* Client socket name */

  /* Converting client_id to the socket name associated with RPC_ID */
  RpcSetClientName(buf, RPC_apicall_in_process_client(idinfo));
  memcpy(client_path_name, buf + 1, 5);
  client_path_name[5] = '\0';
  RpcClientSockNameInfo *current = RPC_sock_info_head(idinfo);

  /* Search info of running client from the management table */
  while (NULL != current) {
    if (0 == strcmp(current->client_sock_name, client_path_name)) {/* Match Running Client Info */
      *client_uid = current->uid;
      *client_gid = current->gid;
      return RPC_OK;
    }
    current = current->next;
  }
  RPC_LOG_ERR("Nothing client credential info.");
  return RPC_ERR_Fatal;  /* Not found Client info */
}

/** @brief Waiting for server authentication
 *
 * When a client requests an API to the server,
 * wait for the server to check whether it is allowed to communicate.
 *
 * @param *idinfo         [IN] Information associated with the client's RPC_ID
 *
 * @retval RPC_OK:                      Authentication OK (communication with the server is permitted)
 * @retval RPC_ERR_Reject_connect:      Authentication NG (Communication denied)
 */
static  RPC_Result
RpcClientWaitCertification(RpcIdInfo *idinfo, pid_t *srvr_pid) {
  fd_set fds;
  RpcCertifyResult recv_ret;  /* Authentication result from the server */
  RPC_Result ret = RPC_ERR_Fatal;

  for(;;) {
    FD_ZERO(&fds);
    FD_SET(RPC_my_secure_sock(idinfo), &fds);

    int sret = select(RPC_my_secure_sock(idinfo) + 1, &fds, NULL, NULL, NULL);
    if (sret < 0 && errno == EINTR) {/* signal interrupt */  // LCOV_EXCL_BR_LINE 5: fail safe for libc select
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      continue;  // LCOV_EXCL_LINE 5: fail safe for libc select
    } else if (sret > 0 && FD_ISSET(RPC_my_secure_sock(idinfo), &fds)) {
      /* Check the response from the server */
      ssize_t rret = recv(RPC_my_secure_sock(idinfo), (char*)&recv_ret, sizeof(RpcCertifyResult), 0);
      if (0 < rret) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc recv
        switch(recv_ret.certify_res) {
          case CERTIFY_OK:  /* OK */
            ret = RPC_OK;
            *srvr_pid = recv_ret.srvr_pid;
          break;

          case CERTIFY_NG:  /* NG */
            ret = RPC_ERR_Reject_connect;
            break;

          default:
            ret = RPC_ERR_Fatal;
            break;
        }
        goto exit_loop_ok;
      } else if(0 == rret) {  // LCOV_EXCL_START 5: fail safe for libc recv
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        RPC_LOG_ERR("server process terminate.");
        return RPC_ERR_Fatal;
      } else {
        RPC_LOG_PERROR("certification_wait recv()");
        return RPC_ERR_Fatal;
      }  // LCOV_EXCL_STOP
    }
  }
exit_loop_ok:
  return ret;
}

/** @brief Register the PID of the communication destination server in the management table.
 *
 * Register a pair of the PID and ID of the server in the management table.
 *
 * @param *idinfo  [IN] Information associated with the client's RPC_ID
 * @param srvr_rpc_id  [IN] RPC_ID of the communication destination server
 * @param srvr_pid  [IN] PID of the communication destination server
 *
 * @retval RPC_OK      : Registration completed
 * @retval RPC_ERR_Fatal          : Registration failed
 */
static  RPC_Result
RpcRegistSrvrPid(RpcIdInfo *idinfo, RPC_ID srvr_rpc_id, pid_t srvr_pid, int wd) {
  if ((NULL == idinfo) || (srvr_rpc_id == RPC_NO_ID) || (0 > srvr_pid)) {  // LCOV_EXCL_START 8: dead code, idinfo always isn't NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_ERR("RpcRegistSrvrPid() : Invalid Param.");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RpcSrvrPidInfo *srvr_pid_buf, *current;

  srvr_pid_buf = rpc_malloc(sizeof(RpcSrvrPidInfo));
  if( srvr_pid_buf == NULL ){  // LCOV_EXCL_START 5: fail safe for libc malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  srvr_pid_buf->srvr_rpc_id = srvr_rpc_id;  /* Server RPC_ID */
  srvr_pid_buf->srvr_pid = srvr_pid;          /* Server PID  */
  srvr_pid_buf->wd = wd;            /* wd    */
  srvr_pid_buf->next = NULL;      /* Next node in the communication destination server PID management table */

  if (0 == RPC_srvr_pid_head(idinfo)) {
    RPC_srvr_pid_head(idinfo) = srvr_pid_buf;
  } else {
    for (current = RPC_srvr_pid_head(idinfo); current->next != NULL; current = current->next)
      ;
    current->next = srvr_pid_buf;
  }
  return RPC_OK;
}

/** @brief Delete Termination Process PID from Management Table
 *
 * Check whether the terminated process matches the communicating server process, and delete it from the management table.
 *
 * @param *idinfo    [IN] Information associated with the client's RPC_ID
 * @param srvr_rpc_id    [IN] Currently communicating server RPC_ID
 * @param wd      [IN] Wd of the termination process
 *
 * @retval RPC_OK    : Deletion completed
 * @retval RPC_ERR_Server_Finish: Deletion completed (server process in communication terminated)
 * @retval RPC_ERR_Fatal        : Argument error
 */
  RPC_Result
RpcDeleteSrvrPid(RpcIdInfo *idinfo, RPC_ID srvr_rpc_id, int wd) {
  if ((NULL == idinfo) || (RPC_NO_ID == srvr_rpc_id)) {  // LCOV_EXCL_START 8: dead code, idinfo always isn't NULL
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_ERR("rpc_check_srvr_pid() : Invalid Param.");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RPC_Result ret = RPC_OK;
  int cnt = 0;
  RpcSrvrPidInfo *current, *previous;
  current = RPC_srvr_pid_head(idinfo);

  while (NULL != current) {
    if (wd == current->wd) {

      /* Check whether the termination process matches the communicating server process */
      if (srvr_rpc_id == current->srvr_rpc_id) {
        ret = RPC_ERR_Server_Finish;
      }

      /* Delete info from the Server info table */
      if (0 == cnt) {/* Delete the start of the management table */
        RPC_srvr_pid_head(idinfo) = RPC_srvr_pid_head(idinfo)->next;
        rpc_free(current);
        current = RPC_srvr_pid_head(idinfo);
        cnt = -1;
      } else {/* Delete all but the first element */
        // LCOV_EXCL_START 8: dead code
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        previous->next = current->next;
        rpc_free(current);
        current = previous->next;
        // LCOV_EXCL_STOP
      }
    } else {
      previous = current;
      current = current->next;
    }
    cnt ++;
  }
  return ret;
}

/** @brief Delete all PID management tables of the communication destination server
 *
 * Releases the PID management table and deletes all data when the client terminates with RPC_end().
 *
 * @param *idinfo      [IN] Information associated with the client's RPC_ID
 *
 * @retval RPC_OK      : Deletion completed
 * @retval RPC_ERR_Fatal          : Argument error
 */
static  RPC_Result
RpcAllDeleteSrvrPid(RpcIdInfo *idinfo) {
  if (NULL == idinfo) {  // LCOV_EXCL_BR_LINE 6: double check in rpc_lib.c --> RPC_end()
    RPC_LOG_ERR("RpcAllDeleteSrvrPid() : Invalid Param.");  // LCOV_EXCL_START 6:  double check in rpc_lib.c --> RPC_end()
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP

  RpcSrvrPidInfo *current = RPC_srvr_pid_head(idinfo);
  while (NULL != current) {

    if (0 <= current->wd) {
      inotify_rm_watch(RPC_clnt_inotify_fd(idinfo), current->wd);
    }

    RpcSrvrPidInfo *previous = current;
    current = current->next;

    rpc_free(previous);
  }
  RPC_srvr_pid_head(idinfo) = NULL;
  return RPC_OK;
}

/** @brief Logging RPC-API calling process
 *
 * Receive File name, function name and line-number of the client calling API and API-function-name, and logging them.
 *
 * @param filename [IN]Pointer to caller filename string
 * @param funcname [IN]Pointer to caller function-name string
 * @param line [IN]Line-number in the source file of the client
 * @param apiname [IN]Pointer to call-API-function-name string
 * @retval 0: normal completion
 * @retval Otherwise: Fatal error
 */
  int
RPC_record_dbg_log( const char *filename, const char *funcname, int line,
    const char *apiname ) {
  printf( "[RPC debug log]%s() called from %s() [%s, line %d]\n",
      apiname, funcname, filename, line );
  return 0;
}

/** @}*/
