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

#ifndef _cl_process_h_  // NOLINT(build/header_guard)
#define _cl_process_h_  // NOLINT(build/header_guard)

#include <sys/types.h>

#define CL_PROCESSS_ATTR_HOLD_FDS_NUM 8

#define CL_INTFY_FILENAME_FORMAT "/tmp/intfy_%05d"

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessAttr_t
/// \~english @par Brief
///        Process attribute structure
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  char body[148];  ///< process attribute
} CL_ProcessAttr_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCleanupInfo_t
/// \~english @par Brief
///        Child-process attribute structure
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  pid_t pid;       ///< child-process ID
  int code;        ///< signal code
  int status;      ///< end status or signal
} CL_ProcessCleanupInfo_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessSchedPolicy_t
/// \~english @par Brief
///        Schedule policy
/////////////////////////////////////////////////////////////////////////////////////
typedef enum {
  CL_PROCESS_SCHED_POLICY_OTHER = 0,  ///< TSS
  CL_PROCESS_SCHED_POLICY_RR,         ///< Round robin
  CL_PROCESS_SCHED_POLICY_FIFO,       ///< FIFO
} CL_ProcessSchedPolicy_t;

typedef struct {
  char body[20];
} CL_ThreadAttr_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttr_t
/// Cgroup attribute structure
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  char body[24];  ///< Cgroup attribute
} CL_ProcessCreateCgroupAttr_t;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file cl_process.h
 * @brief \~english This file contains the base api,enum and struct of cl_process.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup common_library
 *  @ingroup native_service
 *  @{
 */

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessInit
/// \~english @par Brief
///        Initialize the process
/// \~english @retval int file  descriptor
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - Failed to set up process prctl, -1 is returned.
///        - Failed to set the signal set to the empty set sigemptyset, -1 is returned.
///        - Failed to add a signal to the signal mask sigaddset, -1 is returned.
///        - Failed to set sigprocmask, -1 is returned.
///        - Failed to create a scripter in the file for signal acceptance signalfd, -1 is returned.
/// \~english @par Detail
///        This API must be called before createing a thread.
///        Therefore, it is recommended that this API be called from within the main() function immediately after the API started.\n
///        Initialize the process.\n
///        The retuen value fd is assumed to be poll/select and used for waiting for event.\n
///        An ready occurs when the child process terminates.\n
///        Call process name setting(\ref CL_ProcessCreateAttrSetName) uniformly
///        because it is applied only to the number of architecture after exec.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessInit(void);  // NOLINT(readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreate
/// \~english @par Brief
///        Create the process
/// \~english @param [in] file
///        const char*   -
/// \~english @param [in] argv[]
///        char* const  -
/// \~english @param [in] envp[]
///        char* const
/// \~english @param [in] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval int Success(PID)
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Need to be running CL_ProcessInit.
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the executable name (file) in the argument is NULL, set errno to EINVAL, -1 is returned.
///        - When the ARG parameter (argv[]) in the argument is NULL, set errno to EINVAL, -1 is returned.
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When memory (malloc) of the data storagee area of the environment variable parameter (envp[]) specified
///          by the arguments fails, set errno to EINVAL, -1 is returned.
///        - When memory (malloc) of the data control area of the environment variable parameter (envp[]) specified
///          by the arguments fails, set errno to EINVAL, -1 is returned.
///        - Process creation (fork) fail, -1 is returned.
///        - Process CPU affinity masking (sched_setaffinity) fail, -1 is returned.
///        - When the character string output processing (snprintf) for process creation fail, -1 is returned.
///        - When the character string output processing (snprintf) for PID setting of cgroup fail, -1 is returned.
///        - Failure to allocate the area fo sstoring the path name of cgroup (malloc), -1 is returned.
///        - Failure to open file descriptor (open) of cgroup, -1 is returned.
///        - Failure to write file descriptor (write) of cgroup, -1 is returned.
/// \~english @par Detail
///        - This API must be called before createing a thread.
/// \~english @par
///        Create the process.\n
///        The ARG parameter (argv[]) specified in the argment must satisfy the following conditions:\n
///        - Can not be omitted.\n
///        - The first argument must specify a pointer o the filename to be executed.\n
///        - An array of pointer to NULL terminated strings.\n
///        - Arrays must be terminated by NULL pointer.\n
/// \~english @par
///        Enviroment-variable parameters (envp[]) specified in arguments mus satisfy the following condiions:\n
///        - When NULL is specified, the parent process inherits the parent process's environment variables.\n
///        - An array of pointer to NULL terminated strings.\n
///        - Arrays must be terminated by NULL pointer.\n
/// \~english @par
///        Be aware of the following.\n
///        - Processing continues without interrupting event if setrlimit() fail.\n
///        - System call access() checks whether or not there is access privilege,
///          but returns PID regardless of the access privilege status.
///          (If the process does not have access privilege, 100 retries are executed at 100 microseconds.)
/// \~english @par
///        Since the process attribute of a process cannot be changed during processing,
///        if it is necessary to change the process attribute,
///        it is necessary to call the following API and change the process attribute before calling this API.\n
///          - CL_ProcessCreateAttrInit Initialize process attribute
///          - CL_ProcessCreateAttrSetCgroup Setting process attribute (Cgroup)
///          - CL_ProcessCreateAttrSetDisableCloseFds Setting process attribute (stop compulsion FD close)
///          - CL_ProcessCreateAttrSetGid Setting process attribute (group ID)
///          - CL_ProcessCreateAttrSetGroup Setting process attribute (process group)
///          - CL_ProcessCreateAttrSetHoldFds Setting process attribute (maintain FD)
///          - CL_ProcessCreateAttrSetName Setting process attribute (process name)
///          - CL_ProcessCreateAttrSetSchedule Setting process attribute (schedule policy and priority)
///          - CL_ProcessCreateAttrSetStackSize Setting process attribute (Stack Size)
///          - CL_ProcessCreateAttrSetUid Setting process attribute (user ID)
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        CL_ProcessTerminate, CL_ProcessTerminateGroup
///        CL_ProcessCreateAttrInit, CL_ProcessCreateAttrSetCgroup,
///        CL_ProcessCreateAttrSetDisableCloseFds, CL_ProcessCreateAttrSetGid,
///        CL_ProcessCreateAttrSetGroup, CL_ProcessCreateAttrSetHoldFds,
///        CL_ProcessCreateAttrSetName, CL_ProcessCreateAttrSetSchedule,
///        CL_ProcessCreateAttrSetStackSize, CL_ProcessCreateAttrSetUid
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreate(const char *file, char * const argv[], char * const envp[],  // NOLINT(readability/nolint)
                     const CL_ProcessAttr_t *attr);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrInit
/// \~english @par Brief
///        Initialize process attribute
/// \~english @param [out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Initilize the CL_ProcessAttr_t structure that stores process attribute.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Method(Async) / Fire and Forget / Broadcast / Sync / Set Get / Open Close / Request Notify / No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrInit(CL_ProcessAttr_t *attr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetName
/// \~english @par Brief
///        Set process name in process attribute
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] name
///        const char*   - process name (max length 16byte)
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When the pointer (name) to the character string hat stores the process name specified
///          by the argument is NULL, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process name to process attribute.\n
///        The charactr string that stores the process name specified by the argument must satisfy the following conditions:\n
///        - Terminated by a NULL.\n
///        - The maximum lengthof character string is 16 byte including the termination character.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetName(CL_ProcessAttr_t *attr, const char *name);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetUid
/// \~english @par Brief
///        Set process attribute to user ID
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] uid
///        uid_t   - user ID
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to user ID.\n
///        When UINT_MAX is set for the user ID,
///        the user ID is set to 0 in the process attribute in the processing of CL_ProcessCreate().\n
///        When 0 is set for the user ID,
///        it is not reflected in the process attribute becose if is played by the process of CL_ProcessCreate().\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetUid(CL_ProcessAttr_t *attr, uid_t uid);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetGid
/// \~english @par Brief
///        Set process attribute to group ID
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] gid
///        gid_t   - Group ID
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to group ID.\n
///        When 0 is set for the group ID,
///        it is not reflected in the process attribute becose if is played by the process of CL_ProcessCreate().\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetGid(CL_ProcessAttr_t *attr, gid_t gid);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetSchedule
/// \~english @par Brief
///        Set process attribute to schedule policy and priority
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] policy
///        CL_ProcessSchedPolicy_t   - schedule policy
/// \~english @param [in] priority
///        int   - priority
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @par
///        enum CL_ProcessSchedPolicy_t Variable
/// \~english @code
///                 typedef enum {
///                   CL_PROCESS_SCHED_POLICY_OTHER = 0,  // TSS
///                   CL_PROCESS_SCHED_POLICY_RR,         // Round-robin
///                   CL_PROCESS_SCHED_POLICY_FIFO,       // FIFO
///                 } CL_ProcessSchedPolicy_t;
///            @endcode
/// \~english @par
///        target to priority
///        - -20~19 : CL_PROCESS_SCHED_POLICY_OTHER
///        -   1~99 : CL_PROCESS_SCHED_POLICY_RR or CL_PROCESS_SCHED_POLICY_FIFO
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When the schedule policy (policy) specified in the parameter is CL_PROCESS_SCHED_POLICY_RR
///          and the priority (priority) is other than 1 to 99, set errno to EINVAL, -1 is returned.
///        - When the schedule policy (policy) specified in the parameter is CL_PROCESS_SCHED_POLICY_FIFO
///          and the priority (priority) is other than 1 to 99, set errno to EINVAL, -1 is returned.
///        - Incorrect parameters are specified for the schedule policy (policy) specified in the arguments,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to schedule policy and priority.\n
///        Error occur if the real-time schedule policy is set and
///        the priority is passed value outside the range of 1-99.\n
///        In the case of TSS, an error does not occur even if it is outside the range of -20 to 19,
///        but ti is rounded to the nearest integer by a system call when a process is created.\n
///        If not set by this funtion, the schedule policy set the TSS and priority sets 0.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetSchedule(CL_ProcessAttr_t *attr,  // NOLINT(readability/nolint)
                                    CL_ProcessSchedPolicy_t policy,
                                    int priority);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetGroup
/// \~english @par Brief
///        Set process attribute to process group
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] create
///        int   - (0 or 1)
/// \~english @par
///        - 1  make
///        - 0  not make(default)
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When a value oher than 0 or 1 is secified for the process group cration flag (create)
///          specified in the parameter, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to process group.\n
///        When a process group is created, all descendant process fork from child process also belong
///        to the same process group, and the entire process group can be forcibly terminated.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetGroup(CL_ProcessAttr_t *attr, int create);  // NOLINT(readability/nolint)

int CL_ProcessCreateAttrSetCpuAssign(CL_ProcessAttr_t *attr, int cpu_assign);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetStackSize
/// \~english @par Brief
///        Set process attribute to stack size
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] stack_size
///        int   - stack size(byte)
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to stack size.\n
///        The muximum configurable stack size is 1MB.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetStackSize(CL_ProcessAttr_t *attr, int stack_size);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetHoldFds
/// \~english @par Brief
///        Set process attribute to FD to maintain
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] hold_fds[]
///        int   -
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When the array (hode_openfd[]) storing the list of the FD to be maintained specified
///          by the arguments is NULL, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to FD to maintain.\n
///        When nothing is set, FD other than 0, 1, and 2 are automatically close.\n
///        For hold_openfds is stored in a CL_PROCESSS_ATTR_HOLD_FDS_NUM array,
///        it is recommended that int hold_openfds[CL_PROCESSS_ATTR_HOLD_FDS_NUM] be used.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetHoldFds(CL_ProcessAttr_t *attr, int hold_fds[]);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetDisableCloseFds
/// \~english @par Brief
///        Set process attribute to suspend forced FD close
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to suspend forced FD close.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetDisableCloseFds(CL_ProcessAttr_t *attr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateAttrSetCgroup
/// \~english @par Brief
///        Set process attribute to Cgroup
/// \~english @param [in,out] attr
///        const CL_ProcessAttr_t* - Process attribute pointer
/// \~english @param [in] cgroup_name
///        const char*   - Cgroup name
/// \~english @par
///        CL_ProcessAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[148];
///                 } CL_ProcessAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateAttrInit.
///        - cgroup specified by CL_ProcessCreateCgroupCreate() has been created.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When the pointer (name) to the character string hat stores the process name specified
///          by the argument is NULL, set errno to EINVAL, -1 is returned.
///        - When the character string length of the character string (cgroup_name) storing the process name specified
///          by the argument is 64 byte or more, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Set process attribute to Cgroup.\n
///        Creat new Cgroup with CL_ProcessCreateCgroupCreate().\n
///        An error occurs if an Cgroup name that does not exist is specified.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateAttrSetCgroup(CL_ProcessAttr_t *attr,  // NOLINT(readability/nolint)
                                  const char *cgroup_name);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessTerminate
/// \~english @par Brief
///        Kill the process
/// \~english @param [in] pid
///        pid_t - PID
/// \~english @retval int file  descriptor
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - CL_ProcessInit and CL_ProcessCreate must be running.
///          (Process ID to be forcibly terminated exists)
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the process ID to be forcibly terminated does not exist, -1 is returned.
/// \~english @par Detail
///        The PID of the process is he return value (PID) of CL_ProcessCreate().
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        CL_ProcessCreate
/////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessTerminate(pid_t pid);  // NOLINT(readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessTerminateGroup
/// \~english @par Brief
///        Kill process group
/// \~english @param [in] pid
///        pid_t - process group ID
/// \~english @retval int file descriptor
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - CL_ProcessInit and CL_ProcessCreate must be running.
///          (Process ID to be forcibly terminated exists)
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the process ID to be forcibly terminated does not exist, -1 is returned.
/// \~english @par Detail
///        The process group ID is he return value (PID) of CL_ProcessCreate().
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        CL_ProcessCreate
/////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessTerminateGroup(pid_t pid);  // NOLINT(readability/nolint)

int CL_ProcessAbort(pid_t pid);  // NOLINT(readability/nolint)

int CL_ProcessAbortGroup(pid_t pid);  // NOLINT(readability/nolint)

int CL_ProcessEuthanizeGroup(pid_t pid);  // NOLINT(readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCleanup
/// \~english @par Brief
///        Collect child process
/// \~english @param [in] sigchld_fd
///        int - CL_ProcessInit() function return to fd
/// \~english @param [out] cleanup_info
///        CL_ProcessCleanupInfo_t* - Pointer to chiled process info
/// \~english @par
///        CL_ProcessCleanupInfo_t struct
/// \~english @code
///                 typedef struct {
///                   pid_t pid;                /* The process ID of the child. */
///                   int code;                 /* signal code */
///                   int status;               /* the exit status of the child or the signal */
///                 } CL_ProcessCleanupInfo_t;
///            @endcode
/// \~english @retval 0  Success
/// \~english @retval 1  Success
///        (If WNOHANG was specified and no child(ren) specified by id has yet changed state)
/// \~english @retval -1 Error (setting to errno)
/// \~english @par Prerequisite
///        - Must be runnning CL_ProcessInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer (cleanup_info) to the child process information structure is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When a systemu call (waitid) fails, -1 is returned.
///        - When there are no child process in state waitable, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        This API must be called before createing a thread.\n
/// \~english @par
///        It must be called many times while the return valu is 1.
///        After the return value reaches 0, calling it again results in an error (ECHILD).\n
///        The code of CL_ProcessCleanupInfo_t cotains the following value:\n
///          - CLD_EXITED : Finished child process (exited)
///          - CLD_KILLED : Killed child process
///          - CLD_DUMPED : Ended abnormally child process
///          - CLD_TRAPPED : Trapped traced child process
///          - CLD_STOPPED : Stopped child process (stop)
///          - CLD_CONTINUED restarted the sopped child process (after Linux 2.6.9)
///          - The exit status is stored in status only when code is CLD_EXITED. Otherwise, signals are stored.
///          - Need to include signal.h to use CLD_*.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCleanup(int sigchld_fd, CL_ProcessCleanupInfo_t *cleanup_info);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ThreadCreate
/// \~english @par Brief
///        Create thread
/// \~english @param [out] thread
///        pthread_t*   - thread hadle
/// \~english @param [in] attr
///        pthread_attr_t*   - Thread attibute pointer for pthread
/// \~english @param [in] cl_attr
///        CL_ThreadAttr_t*   - Tred attribute pointer for expansion
/// \~english @param [in] start_routine
///        void*(*)(void*)   - Pointer to the entry function of the thread
/// \~english @param [in] arg
///        void*   - Argument to the entry function of the thread
/// \~english @par
///        CL_ThreadAttr_t structure
/// \~english @code
///                 typedef struct {
///                   char body[20];
///                 } CL_ThreadAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error (set errno)
/// \~english @retval EAGAIN failure to create thread
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - When the thread handler (thread) specified in the argument is NULL, set errno to EINVAL, -1 is returned.
///       - When the pointer of the thread attribute specified in the argument is NULL,
///         set errno to EINVAL, -1 is returned.
///       - When the pointer (start_routine) of the entry function of the thread specified by the argument is NULL,
///         set errno to EINVAL, -1 is returned.
///       - Failure to initialize semaphore, set errno to sem_init, -1 is returned.
///       - Failure to create thread (pthread_create), EAGAIN is returned.
///       - Failure to rock semaphore (sem_wait), set errno to sem_init, -1 is returned.
/// \~english @par Detail
///        Create thread.\n
///        The extension thread attribute (cl_attr) is used o assign the thread name set
///        by CL_ThreadCreateAttrSetName(CL_ThreadAttr_t*, const char*).\n
/// \~english @par Classification
///        Public
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ThreadCreate(pthread_t *thread, pthread_attr_t *attr,  // NOLINT(readability/nolint)
                    CL_ThreadAttr_t *cl_attr, void *(*start_routine)(void *),
                    void *arg);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ThreadCreateAttrInit
/// \~english @par Brief
///        Initialize thread attribute
/// \~english @param [out] attr
///        CL_ThreadAttr_t*   - thread attribute pointer
/// \~english @par
///        CL_ThreadAttr_t structure
/// \~english @code
///                 typedef struct {
///                   char body[20];
///                 } CL_ThreadAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error (set errno)
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - When the pointer (attr) of the thread attributespecified by the argument is NULL, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///       Initialize structure CL_ThreadAttr_t that stores threade attribute.\n
/// \~english @par Classification
///        Public
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ThreadCreateAttrInit(CL_ThreadAttr_t *attr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ThreadCreateAttrSetName
/// \~english @par Brief
///        Set thread attribute to thread name
/// \~english @param [out] attr
///        CL_ThreadAttr_t*   - thread attribute pointer
/// \~english @param [in] name
///        const char*   - Pointer to the string storing the thread name (max length 16byte)
/// \~english @par
///        CL_ThreadAttr_t structure
/// \~english @code
///                 typedef struct {
///                   char body[20];
///                 } CL_ThreadAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error (set errno)
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - When the pointer (attr) of thread attribute specified by the argument is NULL, set errno to EINVAL, -1 is returned.
///       - When the pointer (name) to the character string that stores the thread name specified
///         by the argument is NULL, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///       Set thread attribute to thread name\n
///       The characer string that stores the thread name specified by the argument must satisfy the following conditions:\n
///       - Terminated by a NULL.\n
///       - The maximum lengthof character string is 16 byte including the termination character.\n
/// \~english @par Classification
///        Public
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ThreadCreateAttrSetName(CL_ThreadAttr_t *attr, const char *name);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupCreate
/// \~english @par Brief
///        Create Cgroup
/// \~english @param [in] cgroup_name
///        const char* - cgroup name
/// \~english @param [in] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribut
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr by CL_ProcessCreateCgroupAttrInit
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - When the Cgroup name (cgroup_name) specified by the argument is NULL, set errno to EINVAL, -1 is returned.
///       - When the pointer (attr) of the Cgroup attribute specified by the argumet is NULL,
///         set errno to EINVAL, -1 is returned.
///       - When the area for directory path name of the CPU subsystem of Cgroup could not be allocated,
///         set errno to EINVAL, -1 is returned.
///       - When the area for directory of the CPU subsystem of Cgroup could not be create, -1 is returned.
///       - When the area for file path name of the CPU subsystem of Cgroup could not be allocated,
///         set errno to EINVAL, -1 is returned.
///       - When file descriptor of the CPU subsystem of Cgroup could not be acquired, -1 is returned.
///       - When setting value to CPU subsystem of Cgroup could not be written, -1 is returned.
///       - When the area for directory path name of the memory subsystem of Cgroup could not be allocated,
///         set errno to EINVAL, -1 is returned.
///       - When the area for directory of the memory subsystem of Cgroup could not be create, -1 is returned.
///       - When the area for file path name of the memory subsystem of Cgroup could not be allocated,
///         set errno to EINVAL, -1 is returned.
///       - When file descriptor of the memory subsystem of Cgroup could not be acquired, -1 is returned.
///       - When setting value to memory subsystem of Cgroup could not be written, -1 is returned.
/// \~english @par Detail
///        Create Cgroup to which the process to be created belongs.
///        If it belongs to Cgroup that has already been created, it is not necessary to create it.\n
/// \~english @par
///        Since the Cgroup attribute cannot be changed during Cgroup, if it is necessary o chage the Cgroup attribute,
///        it is necessary to call the following API and change the Cgroup attribute calling this API.\n
///          - CL_ProcessCreateCgroupAttrInit :Initialize Cgroup attribute
///          - CL_ProcessCreateCgroupAttrSetCfsBandwidthControl :TSS process CPU time control by Bandwidth Control
///          - CL_ProcessCreateCgroupAttrSetCpuShares :TSS process CPU time control
///          - CL_ProcessCreateCgroupAttrSetMemoryLimit :Contol memory usage
///          - CL_ProcessCreateCgroupAttrSetMemoryUsageNotification :Memory usage notification setting
///          - CL_ProcessCreateCgroupAttrSetRtThrottling :FIFO/RR process CPU time control
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        CL_ProcessCreateCgroupDelete,
///        CL_ProcessCreateCgroupAttrInit, CL_ProcessCreateCgroupAttrSetCfsBandwidthControl,
///        CL_ProcessCreateCgroupAttrSetCpuShares, CL_ProcessCreateCgroupAttrSetMemoryLimit,
///        CL_ProcessCreateCgroupAttrSetMemoryUsageNotification, CL_ProcessCreateCgroupAttrSetRtThrottling,
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupCreate(const char *cgroup_name,  // NOLINT(readability/nolint)
                                 CL_ProcessCreateCgroupAttr_t *attr);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrInit
/// \~english @par Brief
///        Initialize Cgroup attribute
/// \~english @param [out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute pointer
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Prerequisites are nothing.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - When the pointer of Cgroup attribute (attr) specified in the arguments is NULL, set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///       Initialize Cgroup attibute with 0.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrInit(CL_ProcessCreateCgroupAttr_t *attr);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrSetRtThrottling
/// \~english @par Brief
///        Set FIFO/RR process CPU time control
/// \~english @param [in,out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute
/// \~english @param [in] runtime_us
///        int - CPU allocation time(us)
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateCgroupAttrInit.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of Cgroup attribute (attr) specified in the arguments is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Limiting the CPU time allocated to a process to
///        which a real-time class schedule policy (SCHED_FIFO/SCHED_RR) is applied.\n
///        Set the value of argument runtime_us to the cpu.rt_runtime_us (CPU allocation time) of created Cgroup.\n
///        cpu.rt_period_us(unit time:1000000us) is not changed.\n
///        If not set, Cgroup created will remain at default (CPU allocation time:950000us).\n
///        The configurable range of runtime_us depends on the use of Cgroup and the kernel version.
///        The user sets an appropriate value.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrSetRtThrottling(  // NOLINT(readability/nolint)
    CL_ProcessCreateCgroupAttr_t *attr, int runtime_us);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrSetCpuShares
/// \~english @par Brief
///        Set TSS process CPU time control
/// \~english @param [in,out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute pointer
/// \~english @param [in] cpu_shares
///        int - An integer value that sepcified the relative distribution of CPU time
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateCgroupAttrInit
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer (attr) of the Cgroup attribute specified by the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Cotrols the CPU time allocated to a process to which
///        a class TSS schedule policy (SCHED_OTHER, etc.) is applied.\n
///        If cpu_shares is set with this function, a single Cgroup is created and cpu.shares is set with that value.\n
///        If not set, remain at default.\n
///        cpu_shares is set to the standard value 1024, and CPU time is allocated according
///        to the allocaion value specified between Cgroup.\n
///        Task Cgroup with cpu.shares set to 1024 has twice as much CPU time as
///        task Cgroup with cpu.shares set to 512.\n
///        Processes belonging to the same Cgroup are equally allocated CPU time allocated to Cgroup.\n
///        Note that the time distribution is distributed across al CPU cores on a multi-core system.\n
///        In a multicore system, if Cgroup limit is set to 100% of the CPU,
///        100% of each CPU core wil be available.\n\n
///        Examle) If Cgroup A is set to use 25% of the CPU and Cgroup B is set to use 75% of the CPU,\n
///        and a process that uses CPU intensively is started on a 4-core system (1 process in A and 3 processes in B),\n
///        the CPU allocaion is distributed as follows.\n\n
/// \~english @par
///        | PID | cgroup | CPU | CPU allotment |
///        |:---:|:------:|:---:|:-------:|
///        | 100 | A | 0 | CPU0 100% |
///        | 101 | B | 1 | CPU1 100% |
///        | 102 | B | 2 | CPU2 100% |
///        | 103 | B | 3 | CPU3 100% |
/// \~english @par
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrSetCpuShares(CL_ProcessCreateCgroupAttr_t *attr,  // NOLINT(readability/nolint)
                                           int cpu_shares);

///////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrSetCfsBandwidthControl
/// \~english @par Brief
///        Set TS process CU time control by Bandwidth Control
/// \~english @param [in,out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute pointer
/// \~english @param [in] cfs_quota_us
///        int - CPU allocation time
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateCgroupAttrInit
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Controls the amount of time allocated to processes o whitch TSS class schedule plicy (SCHED_OTHER, etc.) are
///        applied using bandwidth control of CFS(Completely Fair Scheduler) implemented from kernel 3.2.\n
///        Set the value of argument cfs_quota_us to the cpu.cfs_quota_us of created Cgroup.\n
///        cpu.cfs_period_us(unit time:1000000us) is not changed.\n
///        If not set, the created Cgroup will not have CPU allocatio control. (set cpu.cfs_guota_us to -1).\n
///        The time (%) allocated to Cgroup can be set correctly,
///        and it can be allocated correctly if the CPU is free.\n
///        It is scheduled to be allocated as much as possible even if it conflicts with other Cgroup's.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrSetCfsBandwidthControl(  // NOLINT(readability/nolint)
    CL_ProcessCreateCgroupAttr_t *attr, int cfs_quota_us);

/////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrSetMemoryLimit
/// \~english @par Brief
///        Control memory usage
/// \~english @param [in,out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute pointer
/// \~english @param [in] memory_limit
///        int - memory limit (byte)
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateCgroupAttrInit
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        Control the CPU time allocated to a process.\n
///        Set the value of argument memory_limit to the memory.limit_in_bytes of created Cgroup.\n
///        If not set, there is no memory limit.When a request exceeding the memory limit is issued,
///        a SIGKILL is sent from the kernel and the process is discarded.
///        Note: When the memory limit is exceeded, control moves to 00M,
///        but its operation ca be changed (Documentation/cgroup/memory.txt).\n
///        Write memory.oom_control to 1 to stop the OOM-Killer.
///        When OOM-Killer is stopped, the task below Cgroup becomes hang/sleep.\n
///        The task moves when the limit of the memory Cgroup increaes or when usage decreases. When usage decreases,\n
///        - Task was killed\n
///        - Task was moved other group\n
///        - File has been deleted\n
/// \~english @par
///       You can also receive a eventf notification from OOM.\n
///       Memory usage is counted as follows\n
///        - RSS(All mapped anon pages)\n
///        - Page Cache\n
///        - First acess Shared Pages in Cgroup\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrSetMemoryLimit(CL_ProcessCreateCgroupAttr_t *attr,  // NOLINT(readability/nolint)
                                             int memory_limit);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupAttrSetMemoryUsageNotification
/// \~english @par Brief
///        Set notificatio when memory usage exceeds the specified amount
/// \~english @param [in,out] attr
///        const CL_ProcessCreateCgroupAttr_t* - Cgroup attribute pointer
/// \~english @param [in] usage_in_bytes
///        int - usage in bytes(byte)
/// \~english @param [in] event_fd
///        int - File dexcriptor created by system call eventfd()
/// \~english @par
///        CL_ProcessCreateCgroupAttr_t struct
/// \~english @code
///                 typedef struct {
///                   char body[24];
///                 } CL_ProcessCreateCgroupAttr_t;
///            @endcode
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Initialize attr with CL_ProcessCreateCgroupAttrInit
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the pointer of the process attribute specified (attr) in the argument is NULL,
///          set errno to EINVAL, -1 is returned.
///        - When event_fd is negative for a file descriptor created by system call eventfd(),
///          set errno to EINVAL, -1 is returned.
/// \~english @par Detail
///        event_fd is notifued when memory usage exceeds argument usage_in_bytes.\n
///        See eventfd for details of event_fd.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupAttrSetMemoryUsageNotification(  // NOLINT(readability/nolint)
    CL_ProcessCreateCgroupAttr_t *attr, int usage_in_bytes, int event_fd);

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupDelete
/// \~english @par Brief
///        Delete Cgroup
/// \~english @param [in] cgroup_name
///        const char* - cgroup name
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Need to be runnning CL_ProcessCreateCgroupCreate.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the Cgroup (cgroup_name) name specified inthe argument is NULL, set errno to EINVAL, -1 is returned.
///        - When the area for directory path name of the CPU subsystem of Cgroup and
///          memory subsystem of Cgroup could not be allocated,  set errno to EINVAL, -1 is returned.
///        - When the area for the directory path name of the memory subsystem of Cgroup cannot be allocated and
///          the directory for the CPU subsystem of Cgroup does not exist, set errno to EINVAL, -1 is returned.
///        - When the directory for the CPU subsystem of Cgroup does not exist and
///          the area for the directory path name of the memory subsystem of Cgroup cannot be allocated,
///          set errno to EINVAL, -1 is returned.
///        - When the directory for the memory subsystem of Cgroup and
///          the directory for the CPU subsystem of Cgroup does not exist, set errno to EINVAL, -1 is returned.
///        - When the directory for the memory subsystem of Cgroup could not be deleted, -1 is returned.
///        - When the directory for the CPU subsystem of Cgroup could not be deleted, -1 is returned.
/// \~english @par Detail
///        Delete Cgroup. If there is a proess belonging to Cgroup, error code (EBUSY) is returned.\n
/// \~english @par
///        Success is also notified in the following cases:\n
///        - When the area for directory path name of memory subsyste Cgroup could not be allocated
///          but CPU subsystem of Cgroup was deleted successfully.
///        - When the directory for the memory subsystem of Cgroup does not exist,
///          but CPU subsystem of Cgroup was deleted successfully.
///        - When the area for directory path name of CPU subsyste Cgroup could not be allocated
///          but memory subsystem of Cgroup was deleted successfully.
///        - When the directory for the CPU subsystem of Cgroup does not exist,
///          but memory subsystem of Cgroup was deleted successfully.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        CL_ProcessCreateCgroupCreate
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupDelete(const char *cgroup_name);  // NOLINT(readability/nolint)

////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CL_ProcessCreateCgroupClassify
/// \~english @par Brief
///        Move process to the Cgroup
/// \~english @param [in,out] cgroup_name
///        const char* - Destination A name
/// \~english @param [in] pid
///        pid_t - process ID
/// \~english @retval 0 Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///        - Need to be running CL_ProcessCreateCgroupCreate.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - When the Cgroup (cgroup_name) specified in the argument is NULL, set errno to EINVAL, -1 is returned.
///        - When the area for directory path name of memory subsystem of Cgroup and
///          CPU subsystem of Cgroup could not be allocated, set errno to EINVAL, -1 is returned.
///        - When the area for directory path name of memory subsystem of Cgroup could not be allocated,
///          and the directory for CPU subsystem of Cgroup does not exist, set errno to EINVAL, -1 is returned.
///        - When the directory for memory subsystem of Cgroup does not exist and
///          the directory for CU subsystem of Cgroup could not allocated , set errno to EINVAL, -1 is returned.
///        - When the directory for the memory subsystem of Cgroup and
///          the directory for the CPU subsystem of Cgroup does not exist, set errno to EINVAL, -1 is returned.
///        - When the area for directory path name of the memory subsystem of Cgroup could not allocated,
///          set errno to EINVAL, -1 is returned.
///        - When the file descripter for the memory subsystem of Cgroup could not be obtained, -1 is returned.
///        - When pid could not be written to the file for memory subsystem of Cgroup, -1 is returned.
///        - When the area for directory path name of the CPU subsystem of Cgroup could not be allocated,
///          set errno to EINVAL, -1 is returned.
///        - When the file decsripter for the CPU subsystem of Cgroup could not be allocated, -1 is returned.
///        - When pid could not be written to the file for CPU subsystem of Cgroup, -1 is returned.
/// \~english @par Detail
///        Move process to the Cgroup.\n
/// \~english @par
///        Success is also notified in the following cases:\n
///        - When the area for directory path name of memory subsyste Cgroup could not be allocated
///          but CPU subsystem of Cgroup was deleted successfully.
///        - When the directory for the memory subsystem of Cgroup does not exist,
///          but CPU subsystem of Cgroup was deleted successfully.
///        - When the area for directory path name of CPU subsyste Cgroup could not be allocated
///          but memory subsystem of Cgroup was deleted successfully.
///        - When the directory for the CPU subsystem of Cgroup does not exist,
///          but memory subsystem of Cgroup was deleted successfully.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        none
////////////////////////////////////////////////////////////////////////////////////
int CL_ProcessCreateCgroupClassify(const char *cgroup_name, pid_t pid);  // NOLINT(readability/nolint)

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif

#endif  // #ifndef _cl_process_h_  // NOLINT(build/header_guard)
