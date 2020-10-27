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
 * @file rpc_event.c
 * @brief RPC Library Internal Implementation--Event Handling
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <native_service/cl_monitor.h>

#include <other_service/rpc.h>
#include "rpc_internal.h"

/** @ingroup RPClib_in
 */
RUNS_IN_READING_THREAD
RPC_Result
RpcQueueAPIRequestBefore(RpcIdInfo *id, UINT32 size, char **buff) {
  RPC_THREAD_MUTEX_LOCK(id->thread_info);

  int n = RPC_apicall_num_queue(id);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_thread.h

  if (n >= RPC_MAX_APICALL_QUEUE) {
    RPC_THREAD_MUTEX_UNLOCK(id->thread_info);
    RPC_LOG_STATE("Returned BUSY.");
    return RPC_ERR_Busy;
  } else {
    *buff = rpc_malloc(size);/* malloc */
    if (*buff == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc function malloc
      RPC_THREAD_MUTEX_UNLOCK(id->thread_info);  // LCOV_EXCL_START 5: fail safe for libc function malloc
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      RPC_LOG_STATE("queue API request: No Memory");
      return RPC_ERR_Fatal;
    }  // LCOV_EXCL_STOP
  }
  return RPC_OK;
}
/** @ingroup RPClib_in
 */
RUNS_IN_READING_THREAD
RPC_Result
RpcQueueAPIRequestAfter(RpcIdInfo *id, RPC_ID client,
          const char *mesg, UINT32 size, char *args) {
  int n = RPC_apicall_num_queue(id);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_thread.h

  UINT16 api_num;
  api_num = (UINT16)strtol(mesg, NULL, 10);
  memcpy(args, mesg + RPC_APICALL_FORMAT_ARGS_START, size);
  RPC_ID_COPY(RPC_apicall_queue_client(id, n), client);
  RPC_apicall_queue_api_num(id, n) = api_num;
  RPC_apicall_queue_args(id, n) = args;
  RPC_apicall_queue_args_size(id, n) = size;
  RPC_apicall_num_queue_inc(id);

  /* set RUN state */
  CL_MonitorSetEntry(CL_MONITOR_TYPE_RPC,
      RPC_port(id),
      CL_MONITOR_STATE_RUN,
      (uint32_t)(RPC_apicall_api_timeout_sec(id)),
      api_num);

  RPC_THREAD_MUTEX_UNLOCK(id->thread_info);
  return RPC_OK;
}

/** @ingroup RPClib_in
 */
RUNS_IN_CALLERS_THREAD
void
RpcFreeAPIArgsString(char *args_string) {
  if (args_string != NULL) {  // LCOV_EXCL_BR_LINE 5: free the memory, malloced in RpcQueueAPIRequestBefore(...)
    rpc_free(args_string);/* free */
  }
}

/** @ingroup RPClib_in
 */
RUNS_IN_CALLERS_THREAD
UINT16
RpcGetAPIRequest(RpcIdInfo *id, RPC_ID_p client,
        char **args_string, unsigned int *args_size) {
  UINT16 api_num = 0;

  RPC_THREAD_MUTEX_LOCK(id->thread_info);

  UINT32 n = RPC_apicall_num_queue(id);  // LCOV_EXCL_BR_LINE 15: marco defined in rpc_thread.h
  if (n > 0) {
    RPC_ID_COPY(*client, RPC_apicall_queue_client(id, 0));
    api_num = RPC_apicall_queue_api_num(id, 0);
    *args_string = RPC_apicall_queue_args(id, 0);
    /* this string must be freed by the caller using discard_APIcall_return()*/
    *args_size = RPC_apicall_queue_args_size(id, 0);

    if (n > 1) {
      memmove(&(RPC_apicall_queue(id, 0)), &(RPC_apicall_queue(id, 1)),
        (n - 1) * sizeof(RPC_apicall_queue(id, 0)));
    }
    RPC_apicall_num_queue_dec(id);
  }

  RPC_THREAD_MUTEX_UNLOCK(id->thread_info);
  return api_num;
}

/** @ingroup RPClib_in
 */
RUNS_IN_READING_THREAD
RPC_Result
RpcSetAPIcallReturn(RpcIdInfo *id, const char *mesg, UINT32 size) {
  if (RPC_apicall_return_str(id) != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
    RPC_LOG_STATE("previous APIcall return string was not used");  // LCOV_EXCL_START 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RpcDiscardAPIcallReturn(id);
  }  // LCOV_EXCL_STOP
  RPC_THREAD_MUTEX_LOCK(id->thread_info);

  RPC_apicall_return_str(id) = rpc_malloc(size);/* malloc */
  if (RPC_apicall_return_str(id) == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc function malloc
    RPC_THREAD_MUTEX_UNLOCK(id->thread_info);  // LCOV_EXCL_START 5: fail safe for libc function malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    RPC_LOG_STATE("set APIcall return: No Memory");
    return RPC_ERR_Fatal;
  }  // LCOV_EXCL_STOP
  RPC_apicall_return_str_len(id) = size;
  memcpy(RPC_apicall_return_str(id), mesg, size);
  RPC_THREAD_MUTEX_UNLOCK(id->thread_info);
  return RPC_OK;
}

/** @ingroup RPClib_in
 */
RUNS_IN_CALLERS_THREAD
void
RpcDiscardAPIcallReturn(RpcIdInfo *id) {
  RPC_THREAD_MUTEX_LOCK(id->thread_info);
  if (RPC_apicall_return_str(id) != NULL) {  // LCOV_EXCL_BR_LINE 6: double check
    rpc_free(RPC_apicall_return_str(id));/* free */
    RPC_apicall_return_str_len(id) = 0;
    RPC_apicall_return_str(id) = NULL;
  }
  RPC_THREAD_MUTEX_UNLOCK(id->thread_info);
}
