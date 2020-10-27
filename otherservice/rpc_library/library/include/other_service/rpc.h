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
 * @file rpc.h
 * @brief \~english RPC library --API define header file
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup other_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup rpc_library
 *  @ingroup other_service
 *  @{
 */
#ifndef OTHERSERVICE_RPC_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_RPC_H_  // NOLINT(build/header_guard)

#include <stdio.h>
#include <unistd.h>

#include <other_service/rpc_id.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
*   @brief \~english RPC_ID max number
*/
#define RPC_MAX_THREADS_IN_PROCESS 64

/** @brief \~english fd max number in one process.
 *  0  = main/sub thread communication pipe fd.
 *  1  =  inotify() process listen fd.
 *  2~129  =  even num : API request data socket.
 *  odd num :  secure stream socket.
 *         */
#define RPC_MAX_FD_IN_PROCESS 130


#define RPC_MAX_API_ARG_NUM 8
///< \~english API call parameter max num


#define RPC_MAX_API_ARG_SIZE 1024
///< \~english API call one parameter max byte

#define RPC_MAX_API_ARG_TOTAL_SIZE 1500
///< \~english API call all parameter max byte

#define RPC_MAX_APICALL_QUEUE 16
///< \~english API call max queue num

#define RPC_NO_PORT 0
///< \~english invalid port num

/** @brief \~english RPC library function return value
 */
enum rpc_result {  // NOLINT (readability/nolint)
  RPC_OK = 0,  ///< \~english ok
  RPC_ERR_No_Response = 0x8fff0000,   ///< \~english has no response
  RPC_ERR_Timeout,   ///< \~english timeout
  RPC_ERR_Busy,  ///< \~english busy
  RPC_ERR_API_Error,  ///< \~english API error
  RPC_ERR_API_Fatal,  ///< \~english API fatal
  RPC_ERR_Fatal,   ///< \~english fatal
  RPC_ERR_Configuration,  ///< \~english configuration
  RPC_ERR_Server_DeadLock,  ///< \~english server deadlock
  RPC_ERR_Server_Finish,   ///< \~english server finish
  RPC_ERR_Reject_connect,   ///< \~english  reject connect
};
/** @brief \~english RPC library function return type
 *         @see rpc_result
 *         */
typedef INT32 RPC_Result;

/** @brief \~english RPC ID
 *
 */
typedef UINT32 RPC_ID;

#define RPC_NO_ID RPC_NO_PORT
///< \~english The ID when destination is invalid

#define RPC_SELF_ID RPC_NO_PORT
///< \~english The ID when destination is self

/** @brief \~english The ID when destination is any
 *         */
#define RPC_ANY_ID RPC_NO_PORT


typedef RPC_Result (*RPC_dispatch_func_t)(UINT16 api_num,
            const char *args_string,
            unsigned int args_size,
            char **ret_string,
            unsigned int *ret_bytes);

#ifdef RPC_DISPATCH_FUNC
#error "Please include <rpc.h> before \"*_srvr.h\"."
#else /* !RPC_DISPATCH_FUNC */
#define RPC_DISPATCH_FUNC NULL
#endif /* !RPC_DISPATCH_FUNC */

/* RPC Library Start Function Macro */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_START_SERVER
/// \~english @par Brief
///     server start(certification of the client by UID)
/// \~english @param [in] ID
///     RPC_ID - program's RPC_ID
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///       - Case of an input parameter error
///       - Case of fail to information generation
///       - When RPC_start has already been issued
///       - Case of space reservation fails
///       - Case of socket generation fails
///       - Case of socket allocation fails
///       - Case of creating a pipe fails
///         - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///       - Used when the server program of RPC-API performs RPC library use start processing
///       - ID specifies a symbol in the form of XXX_RPC_ID defined by rpc_id.h
///       - Only one server program is permitted per thread
///       - When the server program calls the API as a client of another server,
///         it only needs to call RPC_START_SERVER, there is no need to call RPC_START_CLIENT
/// \~english @see RPC_start, RPC_end
////////////////////////////////////////////////////////////////////////////////////
#define RPC_START_SERVER(ID) RPC_start((ID), RPC_DISPATCH_FUNC, NULL, NO_SECURE_CHECK)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_START_SECURE_SERVER
/// \~english @par Brief
///     server start(certification of the client by UID)
/// \~english @param [in] ID
///     RPC_ID - program's RPC_ID
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///       - Case of an input parameter error
///       - Case of fail to information generation
///       - When RPC_start has already been issued
///       - Case of space reservation fails
///       - Case of socket generation fails
///       - Case of socket allocation fails
///       - Case of creating a pipe fails
///         - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///       - Used when the server program of RPC-API performs RPC library use start processing
///       - ID specifies a symbol in the form of XXX_RPC_ID defined by rpc_id.h
///       - Only one server program is permitted per thread
///       - When the server program calls the API as a client of another server,
///         it only needs to call RPC_START_SERVER, there is no need to call RPC_START_CLIENT
/// \~english @see RPC_start, RPC_end
////////////////////////////////////////////////////////////////////////////////////
#define RPC_START_SECURE_SERVER(ID) RPC_start((ID), RPC_DISPATCH_FUNC, NULL, NEED_SECURE_CHECK)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_START_CLIENT
/// \~english @par Brief
///     client start
/// \~english @param [out] pID
///     RPC_ID * - auto allocated program's RPC_ID
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - input parameter error
///     - Unable to get the rpc lib info for the self-thread.
///     - Memory allocation fail for id info.
///     - Socket generation fail.
///     - Socket allocation fail.
///     - Unable to get the socket name.
///         - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - This macro supports only client function of RPC library.
///     - Run the start processing by using this macro only when the APIs provided by the other program are called.
/// \~english @see RPC_start, RPC_end
////////////////////////////////////////////////////////////////////////////////////
#define RPC_START_CLIENT(pID) RPC_start((UINT16)RPC_NO_ID, NULL, (pID), NO_SECURE_CHECK)
//!< \~english client start

/* Client authentication by registering the server's UID list */
#define NEED_SECURE_CHECK 1  ///< \~english  has secure check
#define NO_SECURE_CHECK 0  ///< \~english  has no secure check

/* Setting whether to register UID list of server */
#define REGISTERED 1  ///< \~english registered
#define NO_REGISTERED 0  ///< \~english not registered

/* API Function Prototypes in the RPC Library */
RPC_Result RPC_start(RPC_ID id, RPC_dispatch_func_t func, RPC_ID *pID, INT32 secure_check);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_get_fd
/// \~english @par Brief
///     Get the file descriptor to judge whether there is the request of RPC-API call or not.
/// \~english @param [in] id
///     RPC_ID - server program's RPC_ID
/// \~english @param [out] fd
///     int * - area for fd
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     RPC_START_SERVER or RPC_START_SECURE_SERVER is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Argument(id) is RPC_NO_ID.
///     - Argument(fd) is NULL.
///     - Unable to get the rpc lib info for the self-thread.
///     - RPC sub thread doesn't exist.
///       - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - This function returns the fd(file descriptor) to judge whether there is the request to server.
///     - Return value(fd) is used only for poll function or select function parameter.
///     - Usage
///       - To get fd from this function.
///       - Wait for the data to use poll function or select function.
///       - After that, call RPC_process_API_request function.
///       - As a result, the API call request from client is done(that is, run the server API).
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_get_fd(RPC_ID id, int *fd);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_process_API_request
/// \~english @par Brief
///     Run the processing of RPC-API call
/// \~english @param [in] id
///     RPC_ID - server program's RPC_ID
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     RPC_START_SERVER or RPC_START_SECURE_SERVER is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Argument(id) is RPC_NO_ID.
///     - Unable to get the rpc lib info for the self-thread.
///     - RPC sub thread doesn't exist.
///     - fail to send UDP packet.
///     - API processing from the client returns the error.
///       - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - Process all the queued API running requests from the client.
///     - API function of server program is called in this function.
///     - If there are no requests to run API, this function doesn't wait and return immediately.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_process_API_request(RPC_ID id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_end
/// \~english @par Brief
///     End processing of RPC library.
/// \~english @param [in] id
///     RPC_ID - program's RPC_ID
/// \~english @retval None
/// \~english @par Prerequisite
///     RPC_START_SERVER or RPC_START_SECURE_SERVER or RPC_START_CLIENT is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - End processing(socket close and memory release etc) is done.
///     - When there are no more program that uses RPC_ID, RPC sub thread started by RPC_start will be ended.
///     - Need to call this function before the program is ended.
///     - Need to call this function from which the thread called RPC_start.
///     - Nothing is done when incorrect or no more used RPC_ID is designated.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
void RPC_end(RPC_ID id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_end_all
/// \~english @par Brief
///     End processing of RPC library(forcing to clean up).
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Delete the socket files created in process for communication. /n
///       The program which use RPC library must call RPC_end() to free resource, /n
///       This API is used for insurance processing when socket file's link lost. /n
///       1,Before process ends, call this API. /n
///         When process is ending, auto running socket's close and memory free is not work. /n
///       2,After called this API, all the RPC's process(both server and client) become to useless. /n
///         Even if after called this API, call RPC_START to repair it also can't make sure to active. /n
///       RPC library which use Unix autobind is nothing to do. /n
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
void RPC_end_all(void);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_set_API_timeout
/// \~english @par Brief
///     Set timeout time for server side API processing time
/// \~english @param [in] sec
///     INT32 - timeout time
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @retval RPC_ERR_Configuration Argument is out of range
/// \~english @par Prerequisite
///     RPC_START_SERVER or RPC_START_SECURE_SERVER is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Unable to get the rpc lib info for the self-thread.
///     - Server id info doesn't exist.
///     - Info for API call reception processing doesn't exist.
///       - RPC_ERR_Fatal
///     - The value of the argument(sec) is out of range.
///       - RPC_ERR_Configuration
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_set_API_timeout(INT32 sec);  // NOLINT (readability/nolint)


////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_is_server_ready
/// \~english @par Brief
///       Return RPC server's status
/// \~english @param [in] id
///       RPC_ID  -  server program's RPC_ID
///
/// \~english @retval RPC_OK : Success
/// \~english @retval RPC_ERR_No_Response : No response
///
/// \~english @par Prerequisite
///       - None
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       return the server's work status.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_is_server_ready(RPC_ID id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_regist_credential
/// \~english @par Brief
///     Register UID and GID list
/// \~english @param [in] uid_num
///     int - UID number to regist(number of elements of UID list)
/// \~english @param [in] uid_list
///     uid_t* - head pointer of UID list
/// \~english @param [in] gid_num
///     int - GID number to regist(number of elements of GID list)
/// \~english @param [in] uid_list
///     gid_t* - head pointer of GID list
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @retval RPC_ERR_Configuration Argument is out of range
/// \~english @par Prerequisite
///     RPC_START_SECURE_SERVER is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Unable to get the rpc lib info for the self-thread.
///     - Server id info doesn't exist.
///     - Info for API call reception processing doesn't exist.
///     - Arguments(client_uid and client_gid) are NULL.
///     - Client authentication check is not necessary.
///     - Authentication info is already registered.
///     - Memory allocation fail for UID and GID lists.
///       - RPC_ERR_Fatal
///     - Values of Arguments(uid_num and gid_num) are out of range.
///       - RPC_ERR_Configuration
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - Register the UID and GID lists acceptable by the RPC server.
///     - If RPC_START_SECURE_SERVER is not called, this function returns the error.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_regist_credential(int uid_num, uid_t *uid_list, int gid_num, gid_t *gid_list);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_get_client_credential
/// \~english @par Brief
///     Get UID and GID of client that requested to run APIs processing
/// \~english @param [out] client_uid
///     uid_t* - UID of client that requested to run APIs processing
/// \~english @param [out] client_gid
///     gid_t* - GID of client that requested to run APIs processing
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal Error
/// \~english @par Prerequisite
///     RPC_START_SECURE_SERVER is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Server id info doesn't exist.
///     - Info for API call reception processing doesn't exist.
///     - Arguments(client_uid and client_gid) are NULL.
///     - Client authentication check is not necessary.
///     - There are no running clients.
///     - Info for the running client is not applicable.
///       - RPC_ERR_Fatal
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - This function is to get UID and GID of client that requested to run APIs processing.
///     - If RPC_START_SECURE_SERVER is not called, this function returns the error.
///     - If the running client doesn't exist, this function returns the error.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_get_client_credential(uid_t *client_uid, gid_t *client_gid);  // NOLINT (readability/nolint)

/*
 * The following are only used in stub files
 */
#define RPC_RETCODE_LEN 9

#define RPC_MARSHALL_FLAG_BITS_CODE 12
#define RPC_MARSHALL_FLAG_BITS_IS_VARARRAY 1
#define RPC_MARSHALL_FLAG_BITS_IS_POINTER 1
#define RPC_MARSHALL_FLAG_BITS_IN_OUT 2

/** @brief \~english  APIcall property stored struct
 *
 */
typedef union {
  UINT32 all;  ///< \~english all property
  struct {
    unsigned int code: RPC_MARSHALL_FLAG_BITS_CODE;   ///< \~english value type
    /** @brief \~english  vararray or not
     *
     */
    unsigned int is_vararray: RPC_MARSHALL_FLAG_BITS_IS_VARARRAY;
    /** @brief \~english  pointer or not
     *
     */
    unsigned int is_pointer: RPC_MARSHALL_FLAG_BITS_IS_POINTER;
    /** @brief \~english  in/out parameter property
     *
     */
    unsigned int in_out: RPC_MARSHALL_FLAG_BITS_IN_OUT;
    /** @brief \~english  string or user define type byte num.other is 0
     *
     */
    UINT16 bytes;
  } bits;   ///< \~english variable properties
} RPC_marshall_flag;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_API_call
/// \~english @par brief
///     API call execution function (client)
/// \~english @param [in] id
///     RPC_ID - RPC_ID of server program
/// \~english @param [in] api_num
///     UINT16 - Request API number
/// \~english @param [in] args_string
///     const char* - Argument string
/// \~english @param [in] args_size
///     unsigned int - Argument string length
/// \~english @param [out] ret_string
///     char** - Process result string
/// \~english @param [out] ret_size
///     unsigned int* - Process result string length
/// \~english @retval RPC_OK Normal End
/// \~english @retval RPC_ERR_Fatal Fatal error
/// \~english @retval RPC_ERR_No_Response No response
/// \~english @retval RPC_ERR_Busy API call queue overflow
/// \~english @retval RPC_ERR_Server_DeadLock RPC server deadlock
/// \~english @retval RPC_ERR_Reject_connect The RPC server does not allow communication
/// \~english @par Prerequisite
///     The RPC server is running.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Acquisition of rpc library information for self thread fails
///     - Generation of self-thread rpc library information fails
///     - The RPC ID information does not exist
///     - Failed to receive data from the server at initial authentication
///     - Analysis of received data failed during initial authentication
///     - Commands not expected to be received during initial authentication
///     - Server response when initial authentication is API call error
///     - Failed to receive data from the server at API processing request
///     - Analysis of received data failed during API processing request
///     - Commands not expected to be received during API processing request
///     - Server response when API processing request is API call error
///     - Error in server response other than deadlock when processing results are received
///     - Server shut down
///         - RPC_ERR_Fatal
///     - UDP packet transmission fails at initial authentication
///     - A timeout occurred in response from the server during initial authentication
///     - UDP packet transmission failed at API processing request
///     - A timeout occurred in response from the server during API processing request
///         - RPC_ERR_No_Response
///     - Server response when initial authentication is API call queue overflow
///     - Server response when API processing request is API call queue overflow
///         - RPC_ERR_Busy
///     - When the server response is deadlocked on initial authentication
///     - When the server response is deadlocked on API processing request
///     - Server response is deadlocked when processing result is received
///         - RPC_ERR_Server_DeadLock
///     - Server response is not authenticated at initial authentication
///         - RPC_ERR_Reject_connect
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - Request execution of RPC-API from the client side.
///     - However, since this API is called from the automatically generated code, it is not used directly by the user.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
RPC_Result RPC_API_call(RPC_ID id, UINT16 api_num,  // NOLINT (readability/nolint)
      const char *args_string, unsigned int args_size,
      char **ret_string, unsigned int *ret_size);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_demarshall_arguments
/// \~english @par Brief
///     Confirm whether the input character string conforms to the prescribed format.
/// \~english @param [in] from
///     const char * - Input string
/// \~english @param [in] size
///     unsigned int - Byte length of the input string
/// \~english @param [in] need_alloc
///     int - Flag whether memory allocation is necessary or not.
/// \~english @param [in] num_args
///     int - Number of the variable length arguments
/// \~english @param [in] ...
///     void * - Variable length arguments
/// \~english @retval  0 Normal End
/// \~english @retval -1 Abnormal End
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - Byte length of the input string is 0 bytes or less or larger than 1024 bytes.
///     - The input character string doesn't conform to the prescribed format.
///       - -1
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     This function is used in the stub file only and user must not call this function directly.
/// \~english @see RPC_marshall_arguments
////////////////////////////////////////////////////////////////////////////////////
int RPC_demarshall_arguments(const char *from, unsigned int size,  // NOLINT (readability/nolint)
           int need_alloc, int num_args, ...);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_marshall_arguments
/// \~english @par Brief
///     Collect variable arguments, convert them to a format that matches the receiver's specification,
///     and stuff them into the message buffer.
/// \~english @param [in] size
///     unsigned int * - data size after conversion
/// \~english @param [in] dont_marshall_out_args
///     int - input/output kind(when the argument is a pointer)
/// \~english @param [in] num_args
///     int - value of variable argument
/// \~english @param [in] ...
///     void * - variable argument
/// \~english @retval char * Message buffer after conversion
/// \~english @retval NULL   Abnormal
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     - When marshall work securing work area fails
///     - When the specified type size is abnormal
///     - When securing the area of the message buffer after conversion fails
///       - NULL
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     This function is used in the stub file for client only and user must not call this function directly.
/// \~english @see RPC_demarshalol_arguments
////////////////////////////////////////////////////////////////////////////////////
char *RPC_marshall_arguments(unsigned int *size, int dont_marshall_out_args,  // NOLINT (readability/nolint)
           int num_args, ...);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_marshall_free
/// \~english @par Brief
///     Variadic function for memory release
/// \~english @param [in] num
///     int - number of memory to be released
/// \~english @param [in] ...
///     void * - allocated memory
/// \~english @retval None
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - This function is used in the stub file only and user must not call this function directly.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
void RPC_marshall_free(int num, ...);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_free_return_string
/// \~english @par Brief
///     release memory allocated during running API call.
/// \~english @param [in] ptr
///     void * - allocated memory
/// \~english @retval None
/// \~english @par Prerequisite
///     RPC_API_call is already called.
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync Only
/// \~english @par Detail
///     - This function is used in the stub file for client only and user must not call this function directly.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
void RPC_free_return_string(void *ptr);  // NOLINT (readability/nolint)

#ifdef DBG_ENABLE

////////////////////////////////////////////////////////////////////////////////
/// \ingroup RPC_record_dbg_log
/// \~english @par Brief
///       RPC-API call process's log
/// \~english @param [in] filename
///       const char  -  filename's point
/// \~english @param [in] funcname
///       const char  -  function name's point
/// \~english @param [in] line
///       int         -  file's line
/// \~english @param [in] apiname
///       const char  - called API name's point
/// \~english @param [out]
///
/// \~english @retval 0 : Success
///
/// \~english @par Prerequisite
///       - DBG_ENABLE is defined.
/// \~english @par Change of internal state
///       - None
/// \~english @par Conditions of processing failure
///       - None
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Receipt and record the client's filename, function name,
///       line and called API's function name which called this API.
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////
int RPC_record_dbg_log(const char *filename, const char *funcname, int line,
  const char *apiname);
#endif

#define rpc_malloc malloc
#define rpc_free free

/*
 * RPC debug message control
 * if set to non-null and some critical information is displayed.
 */
extern char *RPC_DEBUG;  // NOLINT (readability/nolint)

/** @}*/  // end of rpc_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem

#ifdef __cplusplus
}
#endif

#endif  // OTHERSERVICE_RPC_H_
