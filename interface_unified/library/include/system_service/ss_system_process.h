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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemServicesIf
/// \brief    This file supports the System Manager OS process abstraction.
///
///////////////////////////////////////////////////////////////////////////////
/**
 * @file ss_system_process.h
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */
#ifndef __Process_H__  // NOLINT (build/header_guard)
#define __Process_H__

#include <spawn.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/cl_process.h>
#include <system_service/ss_system_types.h>
#include <signal.h>
#include <vector>
#include <string>
#include <map>

const long lProcess_VALIDATION_VALUE = 2198645;  // NOLINT (runtime/int)
// Used to define a unique value that represents this class.
// This value is stored in the object when the object is created,
// and cleared when the object is destroyed.

const int iProcess_DEFAULT_PROCESS_PRIORITY = 10;
// This is the default process priority that all new processes will use.

const int iProcess_MAXIMUM_PROCESS_PRIORITY = 60;
// This is the maximum priority a process can have

const long iProcess_DEFAULT_PROCESS_FLAGS = POSIX_SPAWN_SETSCHEDULER  // NOLINT (runtime/int)
    | POSIX_SPAWN_SETSCHEDPARAM;

// This flag field is a bit wise OR of spawning options to use when a process
// is created.

const int iProcess_MAXIMUM_NUMBER_OF_PROCESS_ARGUMENTS = 25;
// This is the maximum number of command line (i.e. argv[]) arguments we can
// send to a process. This includes; exectuable file name + process arguments + NULL

const int iProcess_MAXIMUM_PROCESS_NAME_LENGTH = 32;
// This is the maximum number of characters that a process name can be in bytes.

class Process;

typedef std::map<SS_String, Process *> ProcessMap;

/**
 * @class TimerCtrl
 * \~english @brief OS process
 * \~english @par   Brief Introduction
 *        This class is the System Manager OS process abstraction.
 *
 */
class Process {
 public:
  enum eProcessLoadMode {
    WAIT,   // The invoked program is loaded into available memory, is executed,
    // and then the original program resumes execution.
    NOWAIT,  // Causes the current program to execute concurrently with the new child process.
  };

  enum eProcessSchedulingPolicy {
    FIFO,  // A fixed priority scheduler in which the highest ready process runs until it
    // blocks or is preempted by a higher priority process.
    ROUND_ROBIN,  // The same as FIFO, except processes at the same priority level time-slice.
    OTHER  // A general time sharing scheduler in which a process decays in priority if it
  // consumes too much processor before blocking. It reverts to its default priority
  // when it blocks. Should it fail to run over a 2 second period and it has decayed
  // then it's boosted one priority level up to a maximum of its default priority.
  };

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Process
  /// \~english @par Summary
  ///           - Default Constructor, called when the class is instantiated.
  /// \~english @param  None
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - initialize all var
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - initialize all var in class as default value
  /// \~english @see  ~Process
  ////////////////////////////////////////////////////////////////////////////////////
  Process();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup Process
  /// \~english @par Summary
  ///           - Copy Constructor
  /// \~english @param [in]  - p_rhs_i
  ///           - Process - Reference to a Process that you want to copy.
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Used to create an object and copy another object to the new object.
  /// \~english @see  ~Process
  ////////////////////////////////////////////////////////////////////////////////////
  Process(const Process& p_rhs_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup ~Process
  /// \~english @par Summary
  ///           - Destructor function
  /// \~english @param  None
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - set m_lValidationTag with 0
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Destructor, called when the object is destroyed.\n
  ///             set m_lValidationTag with 0 and kill process.
  /// \~english @see  Process
  ////////////////////////////////////////////////////////////////////////////////////
  virtual ~Process();

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup operator=
  /// \~english @par Summary
  ///           - Assignment Operator
  /// \~english @param [in]
  /// \~english @retval  Process - New Process object
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Assignment Operator, called when one object is assigned to another object.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  Process& operator=(const Process& p_rhs_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetProcessId
  /// \~english @par Summary
  ///           - get process id
  /// \~english @param  None
  /// \~english @retval  process pid - the process pid of the process running.
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - This method will return to the caller the PosixBasedOS001 Specific Process ID.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  int const GetProcessId(void) {
    return static_cast<int>(m_tProcessId);
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetCallingArgumentList
  /// \~english @par Summary
  ///           - This method will set the calling argument list that this object represents
  /// \~english @param [in]  p_pcArgv_i
  ///           - const char *[] - Pointer to array of null terminated parameter list.
  /// \~english @param [in]  p_iArgc_i
  ///           - const int - - Number of arguments in the array passed.
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - put the new arguments into m_strlstArgv
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Set the executable filename first. This is always the 1st argument
  ///           - in the argument list. Then set the argument list for the process
  ///           - which is held in m_strlstArgv.
  ///           - First make sure that the argument list is empty.
  ///           - Once empty, put the new arguments into the list
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetCallingArgumentList(const char *p_pcArgv_i[], const int p_iArgc_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetCallingArgumentList
  /// \~english @par Summary
  ///           - This method will set the calling argument list that this object represents.
  /// \~english @param [in] p_strlstParameters_i
  ///           - const StringList& - String list of arguments that will be used to start.
  /// \~english @retval  None.
  /// \~english @par Preconditions
  ///           - no preconditions.
  /// \~english @par Change of the internal state
  ///           - set m_strlstArgv with p_strlstParameters_i.
  /// \~english @par Causes of failures
  ///           - None.
  /// \~english @par Classification
  ///           - Public.
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - if p_strlstParameters_i is not empty,then clear m_strlstArgv \n
  ///           - and set m_strlstArgv with p_strlstParameters_i.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetCallingArgumentList(const StringList& p_strlstParameters_i);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetExecutableFileName
/// \~english @par Summary
///           - Get executable file name
/// \~english @param  None
/// \~english @retval  Pointer
///                   - Points to SS_String object that holds the path and executable file name for this process
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will return a pointer to the executable filename that this object represents
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  const char * const GetExecutableFileName() {
    return m_strFile.c_str();
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetProcessReturnCode
  /// \~english @par Summary
  ///           - This function will return the processes exit/return code.
  /// \~english @param  None
  /// \~english @retval  process exit code - Code for the last process to execute
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - set m_iReturnCode with WEXITSTATUS(iProcessReturn)
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - This function will return the processes exit/return code.\n
  ///             This is not the value of ERRNO as returned by GetLastPosixBasedOS001ErrorCode().
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  int GetProcessReturnCode(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetSchedulingPolicy
  /// \~english @par Summary
  ///            - This method will change the scheduling policy for the process this object represents.
  /// \~english @param  [in] p_eSchedulingPolicy_i
  ///            - const eProcessSchedulingPolicy - Scheduling Policy for this process
  /// \~english @par
  ///            - eProcessSchedulingPolicy
  /// \~english @code
  ///            - enum eProcessSchedulingPolicy {
  ///                     FIFO,        A fixed priority scheduler in which the highest ready process runs until it\n
  ///                                  blocks or is preempted by a higher priority process.
  ///                     ROUND_ROBIN, The same as FIFO, except processes at the same priority level time-slice.
  ///                     OTHER        A general time sharing scheduler in which a process decays in priority if it\n
  ///                                  consumes too much processor before blocking. It reverts to its default priority\n
  ///                                  when it blocks. Should it fail to run over a 2 second period and it has decayed\n
  ///                                  then it's boosted one priority level up to a maximum of its default priority.
  ///              };
  ///            @endcode
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///            -no preconditions
  /// \~english @par Change of the internal state
  ///            -If the result of function is not 0,then set m_iErrorCode with errno
  ///            - else set m_iErrorCode with 0.
  /// \~english @par Causes of failures
  ///            - None
  /// \~english @par Classification
  ///            - Public
  /// \~english @par Type
  ///            - sync only
  /// \~english @par Detail
  ///            - Attempt to change the scheduling policy for the process that this object
  ///              represents. If the change fails, restore the previous settings.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetSchedulingPolicy(
      const eProcessSchedulingPolicy p_eSchedulingPolicy_i);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup GetSchedulingPolicy
/// \~english @par Summary
///           - Get currently configured scheduling policy
/// \~english @param  None
/// \~english @retval  FIFO         - A fixed priority scheduler in which the highest ready process runs until it\n
///                                   blocks or is preempted by a higher priority process.
/// \~english @retval  ROUND_ROBIN  - The same as FIFO, except processes at the same priority level time-slice.
/// \~english @retval  OTHER        - A general time sharing scheduler in which a process decays in priority if it\n
///                                   consumes too much processor before blocking. It reverts to its default priority\n
///                                   when it blocks. Should it fail to run over a 2 second period and it has decayed\n
///                                   then it's boosted one priority level up to a maximum of its default priority.
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - If the result of calling function sched_getscheduler(m_tProcessId) is -1
///             then set m_iErrorCode with errno
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will return to the caller the currently configured process scheduling policy.
///             If the result of calling function sched_getscheduler(m_tProcessId) is -1,
///             then return ROUND_ROBIN.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  eProcessSchedulingPolicy const GetSchedulingPolicy(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetPriority
  /// \~english @par Summary
  ///           - This method will change the priority for the process this object represents.
  /// \~english @param  [in] p_iPriority_i
  ///           - int - Priority of the process
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - If the result of function sched_setparam(m_tProcessId, &cur_sch_params)\n
  ///             is -1,then set m_iErrorCode with errno, else set m_iErrorCode with 0.
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Using p_iPriority_i to change the priority for the process\n
  ///             this object represents,if the result of function \n
  ///             sched_setparam(m_tProcessId, &cur_sch_params) is -1
  ///             m_iErrorCode will be seted with errno,else be seted with 0
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetPriority(const int p_iPriority_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetPriority Notice:Can't find the body of this function
  /// \~english @par Summary
  ///           - This method will change the priority for the specified process ID.
  /// \~english @param  [in] p_iPid_i
  ///           - pid_t - PID of the process to change the priority
  /// \~english @param  [in] p_iPriority_i
  ///           - int - Priority of the process
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           -
  /// \~english @par Causes of failures
  ///           -
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           -none
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetPriority(const pid_t p_iPid_i, const int p_iPriority_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup IncreasePriorityByOne
  /// \~english @par Summary
  ///            - This method will increase the priority for the process this\n
  ///              object represents by one.
  /// \~english @param  None
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Retrieve the current priority of the process. Check to see if already at max.\n
  ///             If so just return. Otherwise increase by one and set the priority...
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void IncreasePriorityByOne(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup DecreasePriorityByOne
  /// \~english @par Summary
  ///           - This method will decrease the priority for the process this object represents by one.
  /// \~english @param  None
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no precondition
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - Retrieve the current priority of the process. Check to see if already at minimum.
  ///             If so just return. Otherwise decrease by one and set the priority...
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void DecreasePriorityByOne(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetPriority
  /// \~english @par Summary
  ///           - This method will return to the caller the currently configured process priority.
  /// \~english @param  None
  /// \~english @retval  int priority - process priority
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - If the result of function sched_getparam(m_tProcessId, &cur_sch_params) is less than -1,\n
  ///             then m_iErrorCode will be seted with errno.
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - If the result of function sched_getparam(m_tProcessId, &cur_sch_params)\n
  ///             is less than -1,will be return -1,else return Currently configured process priority.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  int const GetPriority(void);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SetProcessName
  /// \~english @par Summary
  ///            - This method will set this objects process name member variable to the provided string value.
  /// \~english @param  [in] p_strProcessName_i
  ///            - SS_String - Process Name to set the m_strProcessName member variable to
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///            -no preconditions
  /// \~english @par Change of the internal state
  ///            - Using p_strProcessName_i to set m_strProcessName
  /// \~english @par Causes of failures
  ///            - None
  /// \~english @par Classification
  ///            - Public
  /// \~english @par Type
  ///            - sync only
  /// \~english @par Detail
  ///            - This method will set this objects process name member variable to the provided string value.\n
  ///              If the user of this object wishes to register the name with PosixBasedOS001 OS,\n
  ///              the AttachName() method should be called.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void SetProcessName(const SS_String& p_strProcessName_i);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup GetProcessName
  /// \~english @par Summary
  ///           - Get process name
  /// \~english @param  None
  /// \~english @retval  SS_String - Process Name
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - This method will return to the caller the registered name of this process.\n
  ///             been registered with the OS, a NULL string will be returned. If a name has not
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  SS_String GetProcessName(void) const {
    return m_strProcessName;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateProcess
  /// \~english @par Summary
  ///            - This method will create a process with the executable provided and \n
  ///              mode as a calling parameter.The caller can also provide a list of arguments \n
  ///              that will be provided to the executable at startup.
  /// \~english @param  [in] p_strFile_i
  ///            - SS_String - Path and Filename of executable to create process for
  /// \~english @param  [in] p_strlstArgv_i
  ///            - StringList - List of ARGV values for new process
  /// \~english @param  [in] p_eMode_i
  ///            - eProcessLoadMode - Mode to create and load new process
  /// \~english @param  [in] p_strProcessName_i
  ///            - SS_String - This is the name that will be registered to the OS for this process
  /// \~english @param  [in] p_eSchedulingPolicy_i
  ///            - eProcessSchedulingPolicy
  /// \~english @param  [in] p_iPriority_i
  ///            - int - Priority for this process
  /// \~english @param  [in] unix_user_name
  ///            - char* - unix user name
  /// \~english @param  [in] p_lSpawnFlags_i
  ///            - long - Spawning flags. These are PosixBasedOS001 specific....
  /// \~english @par
  ///            eProcessLoadMode enum
  /// \~english @code
  ///            - enum eProcessLoadMode {
  ///                WAIT,        //The invoked program is loaded into available memory, is executed,
  ///                             //and then the original program resumes execution.
  ///                NOWAIT,      // Causes the current program to execute concurrently with the new child process.
  ///              };
  ///            @endcode
  /// \~english @par
  ///            eProcessSchedulingPolicy enum
  /// \~english @code
  ///            - enum eProcessSchedulingPolicy {
  ///                FIFO,         // A fixed priority scheduler in which the highest ready process runs until it
  ///                              // blocks or is preempted by a higher priority process.
  ///                ROUND_ROBIN,  // The same as FIFO, except processes at the same priority level time-slice.
  ///                OTHER         // A general time sharing scheduler in which a process decays in priority if it
  ///                              // consumes too much processor before blocking. It reverts to its default priority
  ///                              // when it blocks. Should it fail to run over a 2 second period and it has decayed
  ///                              // then it's boosted one priority level up to a maximum of its default priority.
  ///             };
  ///            @endcode
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - set m_iErrorCode with 0
  ///             set m_iReturnCode with 0
  ///             set m_eProcessLoadMode with p_eMode_i;
  ///             set m_strFile with p_strFile_i;
  ///             set m_strProcessName with p_strProcessName_i;
  ///             use the result of function CL_ProcessCreate() to set m_tProcessId
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///            -This method will create a PosixBasedOS001 process with the executable provided\n
  ///             and mode as a calling parameter.The caller can also provide a list of arguments\n
  ///             that will be provided to the executable at startup.The calling p_strProcessName \m
  ///             parameter is a textual name that will be associated with the newly created process\n
  ///             by the OS. The process state information will be maintained by this object.\n
  ///             Upon successful creation of the process, the scheduling policy and priority\n
  ///             of the process will be set to the provided values. The user can change these\n
  ///             values through the SetSchedulingPolicy() and SetPriority() method calls.\n
  ///             Using unix_user_name to get uid and gid,set uid and gid when calling\n
  ///             CL_ProcessCreateAttrSetUid() and CL_ProcessCreateAttrSetGid\n
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const StringList& p_strlstArgv_i,   // List of ARGV values for new process
      const eProcessLoadMode p_eMode_i,   // Mode to create and load new process
      const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
      const eProcessSchedulingPolicy p_eSchedulingPolicy_i,  // Scheduling Policy for this process
      const int p_iPriority_i,        // Priority for this process
      const char* unix_user_name, const long p_lSpawnFlags_i);  // Spawning flags. These are PosixBasedOS001 specific....  // NOLINT (runtime/int)

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateProcess
  /// \~english @par Summary
  ///            - This method will create a PosixBasedOS001 process with the executable\n
  ///              provided and mode as a calling parameter.
  /// \~english @param  [in] p_strFile_i
  ///            - SS_String - Path and Filename of executable to create process for
  /// \~english @param  [in] p_strProcessName_i
  ///            - SS_String - This is the name that will be registered to the OS for this process
  /// \~english @param  [in] unix_user_name
  ///            - char* - unix user name
  /// \~english @param  [in] p_lSpawnFlags_i
  ///            - long - Posix Spawning flags. These are PosixBasedOS001 specific
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - The caller can also provide a list of arguments that will be provided\n
  ///             to the executable at startup.The calling p_strProcessName parameter is a textual name\n
  ///             that will be associated with the newly created process by the OS.\n
  ///             The process state information will be maintained by this object.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
      const char* unix_user_name = NULL, const long p_lSpawnFlags_i =  // NOLINT (runtime/int)
          iProcess_DEFAULT_PROCESS_FLAGS);  // Spawning flags.
                                            // These are PosixBasedOS001 specific....
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateProcess
  /// \~english @par Summary
  ///           - This method will create a process with the executable
  ///           provided and mode as a calling parameter.
  /// \~english @param  [in] p_strFile_i
  ///            - SS_String - Path and Filename of executable to create
  ///            process for
  /// \~english @param  [in] p_strProcessName_i
  ///            - SS_String - This is the name that will be registered to the
  ///            OS for this process
  /// \~english @param  [in] p_iPriority_i
  ///            - int - Priority of process
  /// \~english @param  [in] unix_user_name
  ///            - char* - unix user name
  /// \~english @param  [in] p_lSpawnFlags_i
  ///            - long - Posix Spawning flags. These are PosixBasedOS001
  ///            specific
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - The caller can also provide a list of arguments that will be
  ///           provided\n
  ///             to the executable at startup.The calling p_strProcessName
  ///             parameter is a textual name\n
  ///             that will be associated with the newly created process by
  ///             the OS.\n
  ///             The process state information will be maintained by this
  ///             object.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
      const int p_iPriority_i,        // Priority of this process
      const char* unix_user_name = NULL, const long p_lSpawnFlags_i =  // NOLINT (runtime/int)
          iProcess_DEFAULT_PROCESS_FLAGS);  // Spawning flags.
                                            // These are PosixBasedOS001 specific....

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CreateProcess
/// \~english @par Summary
///           - This method will create a process with the executable provided and mode as a calling parameter.
/// \~english @param  [in] p_strFile_i
///            - SS_String - p_strFile_i Path and Filename of executable to create process for
/// \~english @param  [in] unix_user_name
///            - char* - unix user name
/// \~english @param  [in] p_lSpawnFlags_i
///            - long - Spawning flags. These are PosixBasedOS001 specific.
/// \~english @retval  None
/// \~english @par Preconditions
///           -
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - The caller can also provide a list of arguments that will be provided\n
///             to the executable at startup.The calling p_strProcessName parameter is a textual name\n
///             that will be associated with the newly created process by the OS.\n
///             The process state information will be maintained by this object.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const char* unix_user_name = NULL, const long p_lSpawnFlags_i =  // NOLINT (runtime/int)
          iProcess_DEFAULT_PROCESS_FLAGS);  // Spawning flags.
                                            // These are PosixBasedOS001 specific....

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CreateProcess
/// \~english @par Summary
///           - This method will create a process with the executable provided and mode as a calling parameter.
/// \~english @param  [in] p_strFile_i
///            - SS_String - p_strFile_i Path and Filename of executable to create process for
/// \~english @param  [in] p_strProcessName_i
///            - SS_String - This is the name that will be registered to the OS for this process
/// \~english @param  [in] p_strlstArgv_i
///            - StringList - List of ARGV values for new process
/// \~english @param  [in] unix_user_name
///            - char* - unix user name
/// \~english @param  [in] p_lSpawnFlags_i
///            - long - Spawning flags. These are PosixBasedOS001 specific.
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - The caller can also provide a list of arguments that will be provided\n
///             to the executable at startup.The calling p_strProcessName parameter is a textual name\n
///             that will be associated with the newly created process by the OS.\n
///             The process state information will be maintained by this object.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
      const StringList& p_strlstArgv_i,   // List of ARGV values for new process
      const char* unix_user_name = NULL, const long p_lSpawnFlags_i =  // NOLINT (runtime/int)
          iProcess_DEFAULT_PROCESS_FLAGS);  // Spawning flags.
                                            // These are PosixBasedOS001 specific....

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CreateProcess
/// \~english @par Summary
///           - This method will create a  process with the executable provided and mode as a calling parameter.
/// \~english @param  [in] p_strFile_i
///            - SS_String - Path and Filename of executable to create process for
/// \~english @param  [in] p_strProcessName_i
///            - SS_String - This is the name that will be registered to the OS for this process
/// \~english @param  [in] p_iPriority_i
///            - int - Priority for this process
/// \~english @param  [in] p_strlstArgv_i
///            - StringList - List of ARGV values for new process
/// \~english @param  [in] unix_user_name
///            - char* - unix user name
/// \~english @param  [in] p_lSpawnFlags_i
///            - long - Spawning flags.These are PosixBasedOS001 specific.
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - The caller can also provide a list of arguments that will be provided\n
///             to the executable at startup.The calling p_strProcessName parameter is a textual name\n
///             that will be associated with the newly created process by the OS.\n
///             The process state information will be maintained by this object.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
      const int p_iPriority_i,        // Priority for this process
      const StringList& p_strlstArgv_i,   // List of ARGV values for new process
      const char* unix_user_name = NULL, const long p_lSpawnFlags_i =  // NOLINT (runtime/int)
          iProcess_DEFAULT_PROCESS_FLAGS);  // Spawning flags.
                                            // These are PosixBasedOS001 specific....
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CreateProcess
  /// \~english @par Summary
  ///           - This method will create a  process with the executable provided and mode as a calling parameter.
  /// \~english @param  [in] p_strFile_i
  ///            - SS_String - Path and Filename of executable to create process for
  /// \~english @param  [in] c_argv
  ///            - char - This is the argument that will be registered to the OS for this process
  /// \~english @param  [in] environment_string
  ///            - char - enviroment for this process
  /// \~english @param  [in] cl_attr
  ///            - CL_ProcessAttr_t - List of ARGV values for new process
  /// \~english @retval  None
  /// \~english @par Preconditions
  ///           -no preconditions
  /// \~english @par Change of the internal state
  ///           - None
  /// \~english @par Causes of failures
  ///           - None
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par Detail
  ///           - The caller can also provide a list of arguments that will be provided\n
  ///             that will be associated with the newly created process by the OS.\n
  ///             The process state information will be maintained by this object.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
  void CreateProcess(const SS_String* p_str_file, char* const * c_argv,
                     char* environment_string, const CL_ProcessAttr_t *cl_attr);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CreateProcessWait
/// \~english @par Summary
///           - This method will create a process with the executable provided.
/// \~english @param  [in] p_strFile_i
///            - SS_String - Path and Filename of executable to create process for
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will create a process with the executable provided.\n
///             The process state information will be maintained by this object.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void CreateProcessWait(const SS_String& p_strFile_i);  // Path and Filename of executable to create process for

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup CreateProcessWait
/// \~english @par Summary
///           - This method will create a PosixBasedOS001 process with the executable provided.
/// \~english @param  [in] p_strFile_i
///            - SS_String - Path and Filename of executable to create process for
/// \~english @param  [in] p_strlstArguments_i
///            - StringList - List of process calling arguments
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - None
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will create a PosixBasedOS001 process with the executable provided.\n
///             The process state information will be maintained by this object.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void CreateProcessWait(const SS_String& p_strFile_i,  // Path and Filename of executable to create process for
      const StringList& p_strlstArguments_i);  // List of process calling arguments

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup KillProcess
/// \~english @par Summary
///           - This method will send the specified signal to the process represented by this object
/// \~english @param  [in] signal
///            - int - signal
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - Intialize the objects m_iErrorCode member variable to 0
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will send the specified signal to the process represented by this object.\n
///             All variables associated with this object will be initialized to a know value.\n
///             If this process has a name registered with the OS, that name will be removed.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void KillProcess(int signal = SIGKILL);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DoesProcessExist
/// \~english @par Summary
///           -Check if the process existed
/// \~english @param  None
/// \~english @retval TRUE  - Process Exists
/// \~english @retval FALSE - Process does not exist
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - The internal state is not changed.
/// \~english @par Causes of failures
///           -None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will return a BOOLean indicating whether this process exists.
/// \~english @see
////////////////////////////////////////////////////////////////////////////////////
  BOOL DoesProcessExist(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup DisableAutoKill
/// \~english @par Summary
///           -none
/// \~english @param  None
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - set m_fAutoKill as FALSE
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will disable the Auto Kill function in the destructor,\n
///             thus letting the process whom this object represents continue running.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void DisableAutoKill(void) {
    m_fAutoKill = FALSE;
  }

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EnableAutoKill
/// \~english @par Summary
///           - Enable the Auto Kill
/// \~english @param  None
/// \~english @retval  None
/// \~english @par Preconditions
///           -no preconditions
/// \~english @par Change of the internal state
///           - set m_fAutoKill as TRUE
/// \~english @par Causes of failures
///           - None
/// \~english @par Classification
///           - Public
/// \~english @par Type
///           - sync only
/// \~english @par Detail
///           - This method will Enable the Auto Kill function in the destructor,\n
///             thus killing the process whom this object represents.
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
  void EnableAutoKill(void) {
    m_fAutoKill = TRUE;
  }

 private:
  //  Class Data Members
  //---------------------------------------------

  long m_lValidationTag;  // Used to check if this is a valid  // NOLINT (runtime/int)
  // object or not.

  pid_t m_tProcessId;  // This is the process ID for this process. All command, control,
  // and status gathering will use this ID.
  eProcessLoadMode m_eProcessLoadMode;  // This is the process load mode provided by the caller when this
  // process was created.
  SS_String m_strFile;  // This is the executable path and file name of this process
  StringList m_strlstArgv;  // This is the calling parameter Argv list provided by the caller.
  SS_String m_strProcessName;  // This is the name that was registered to the OS for process identification

  int m_iErrorCode;  // This was the returned PosixBasedOS001 error code for the last call. The programmer
  // can look at this error code if an error was detected and an
  // exception was thrown.
  BOOL m_fAutoKill;  // Used to indicate to destructor to kill the process whom this object
  // represents on exit.
  int m_iReturnCode;

  //////////////////////////////////////////////////////////////////////
  /// Copy
  ///
  /// Copies data members from the specified object to this object.
  /// No attempt is made to free dynamically allocated objects within
  /// this object (you must do that before calling this function).
  ///
  ///////////////////////////////////////////////////////////////////////
  void Copy(const Process& p_rhs_i);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// ConvertToPosixBasedOS001SchedularPolicy
  /// This method will return to the caller the equivalent PosixBasedOS001 schedular policy for the process
  /// that this object represents
  ///
  ///
  /// Calling Arguments:
  ///      Scheduling Policy
  ///
  /// \return policy
  ///      FIFO,       A fixed priority scheduler in which the highest ready process runs until it
  ///               blocks or is preempted by a higher priority process.
  ///      ROUND_ROBIN,   The same as FIFO, except processes at the same priority level time-slice.
  ///      OTHER       A general time sharing scheduler in which a process decays in priority if it
  ///               consumes too much processor before blocking. It reverts to its default priority
  ///               when it blocks. Should it fail to run over a 2 second period and it has decayed
  ///               then it's boosted one priority level up to a maximum of its default priority.
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  int const ConvertToPosixBasedOS001SchedularPolicy(
      const eProcessSchedulingPolicy p_eSchedulingPolicy_i);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// ConvertFromPosixBasedOS001SchedularPolicy
  ///
  /// This method will return to the caller the eProcessSchedulingPolicy based on the PosixBasedOS001 schedular
  /// policy for the process that this object represents
  ///
  ///
  /// Calling Arguments:
  ///      PosixBasedOS001 Scheduling Policy
  ///
  /// \return policy
  ///      FIFO,      A fixed priority scheduler in which the highest ready process runs until it
  ///              blocks or is preempted by a higher priority process.
  ///      ROUND_ROBIN,  The same as FIFO, except processes at the same priority level time-slice.
  ///      OTHER      A general time sharing scheduler in which a process decays in priority if it
  ///              consumes too much processor before blocking. It reverts to its default priority
  ///              when it blocks. Should it fail to run over a 2 second period and it has decayed
  ///              then it's boosted one priority level up to a maximum of its default priority.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  eProcessSchedulingPolicy const ConvertFromPosixBasedOS001SchedularPolicy(
      const int p_iPosixBasedOS001chedulingPolicy_i);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemServicesIf
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Check environment variable MOCK_LIBRARY where Mock Library Name to set in LD_PRELOAD is set
  ///        and output character string to set in LD_PRELOAD if there is setting.
  /// \return
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void CheckLdPreLoad(SS_String *process_path, char *environment_string);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup SS_SystemServicesIf
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Acquire the character string of the environment variable and set it in String Vector
  ///        and return in a pointer of Vector.
  /// \return vector<SS_String> pointer
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<SS_String> *GetEnvironVector(void);
// ----------------------------------------
// ---------------- End -------------------
};
#endif  // NOLINT (build/header_guard)

/** @}*/
/** @}*/
/** @}*/
