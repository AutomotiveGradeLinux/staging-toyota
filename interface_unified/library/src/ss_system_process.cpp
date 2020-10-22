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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManagerIf
/// \brief    This file provides support for the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#include "system_service/ss_system_process.h"
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <elfio/elfio_dump.hpp>

#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "ss_system_if_interfaceunifiedlog.h"

const char* iProcess_DEFAULT_PROCESS_USER_NAME = "default_user";
uid_t iProcess_DEFAULT_PROCESS_USER = 4999;
gid_t iPrpocess_DEFAULT_PROCESS_GROUP = 4999;

/**
 * Process: Called when the class is instantiated.
 *
 * @return 
 */
Process::Process(int cpu_assign) :
  m_lValidationTag(lProcess_VALIDATION_VALUE),
  m_cpu_assign(cpu_assign),
  m_tProcessId(-1),
  m_eProcessLoadMode(NOWAIT),
  m_strFile(""),
  m_strProcessName(""),
  m_iErrorCode(0),
  m_fAutoKill(TRUE),
  m_iReturnCode(0) {
}


/**
 * ~Process: Called when the object is destroyed.
 *
 * @return 
 */
Process::~Process() {
  // ASSERT_VALID (this);

  //
  // Set the validation tag to NOT valid
  m_lValidationTag = 0;

  //
  // Cleanup after ourselves...
  //
  if ((m_fAutoKill) && (m_tProcessId != -1) && (m_tProcessId != getpid())) {
    KillProcess();        // Then remove the process from PosixBasedOS001
  }
}

/**
 * Process: Used to create an object and copy another
 * object to the new object.
 *
 * @param p_rhs_i Reference to the class being copied from. 
 * @return Process( const
 */
Process::Process(const Process& p_rhs_i) {
  // Copy data from the specified object to this object.
  Copy(p_rhs_i);
}


/**
 * operator=: Called when one object is assigned 
 * to another object.
 *
 * @param p_rhs_i Reference to the class being copied from. 
 * @return Process&
 */
Process& Process::operator= (const Process& p_rhs_i) {
  // Don't do anything if we're being copied on to ourselves.
  if (this == &p_rhs_i) return (*this);

  // Copy data from the specified object to this object.
  Copy(p_rhs_i);

  return (*this);
}


/**
 * Copy: Copies data members from the specified object to this object.
 * No attempt is made to free dynamically allocated objects within
 * this object (you must do that before calling this function).
 *
 * @param p_rhs_i 
 * @return void
 */
void Process::Copy(const Process& p_rhs_i) {
  // Copy data from the specified object to this object.
  m_lValidationTag = p_rhs_i.m_lValidationTag;

  m_tProcessId = p_rhs_i.m_tProcessId;
  m_eProcessLoadMode = p_rhs_i.m_eProcessLoadMode;
  m_strFile = p_rhs_i.m_strFile;
  m_strlstArgv = p_rhs_i.m_strlstArgv;
  m_strProcessName = p_rhs_i.m_strProcessName;
  m_iErrorCode = p_rhs_i.m_iErrorCode;
  m_fAutoKill = p_rhs_i.m_fAutoKill;
  m_iReturnCode = p_rhs_i.m_iReturnCode;
}

/**
 * GetProcessReturnCode: This function will return the processes
 * exit/return code. This is not the value of ERRNO as returned by 
 * GetLastPosixBasedOS001ErrorCode().
 *
 * @return code from the process
 */
int Process::GetProcessReturnCode() {
  int iProcessReturn = 0;
  if (waitpid(m_tProcessId, &iProcessReturn, WNOHANG) > 0) {
    m_iReturnCode = WEXITSTATUS(iProcessReturn);
  }

  return m_iReturnCode;
}

static int getIdsFromUserName(const char* user_name, uid_t *uid, gid_t *gid) {
  int ret = -1;

  try {
    if (NULL == user_name) {
      *uid = geteuid();
      *gid = getegid();
    } else {
      static __thread size_t  bufSize = 0;
      static __thread char   *buf = NULL;
      struct passwd  pwd;
      struct passwd *result;

      if (0 == bufSize) {
        struct stat statInfo;

        bufSize = -1;

        if (0 != stat("/etc/passwd", &statInfo) || 0 >= statInfo.st_size) {
          throw "/etc/passwd is not exist";
        }

        bufSize = statInfo.st_size * 2;

        // Since SystemManager is a resident service,
        // the area secured here is not explicity released by free()
        // in anticipation of release at the end of the process
        buf = reinterpret_cast<char *>(malloc(sizeof(char) * bufSize));
        if (NULL == buf) {
          throw "malloc()";
        }
      }

      if (NULL == buf) {
        throw "buf = NULL";
      }

      ret = getpwnam_r(user_name, &pwd, buf, bufSize, &result);
      if (ret == 0 && result != NULL) {
        *uid = pwd.pw_uid;
        *gid = pwd.pw_gid;
      } else {
        throw "getpwnam_r()";
      }
    }

    ret = 0;
  }

  catch (const char *e) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "ERROR:%s", e);
  }

  return ret;
}

/**
 * CreateProcess: This method will create a PosixBasedOS001 process with the executable provided and mode as a calling parameter. 
 * The caller can also provide a list of arguments that will be provided to the executable at startup. 
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information 
 * will be maintained by this object.
 *
 * Upon successful creation of the process, the scheduling policy and priority
 * of the process will be set to the provided values. The user can change these
 * values through the SetSchedulingPolicy() and SetPriority() method calls.
 *
 * @param p_strFile_i Path and Filename of executable to create process for
 * @param p_strlstArgv_i List of ARGV values for new process
 * @param p_eMode_i Mode to create and load new process
 *          WAIT     -  The invoked program is loaded into available memory, is executed,
 *                      and then the original program resumes execution.
 *          NOWAIT   -  Causes the current program to execute concurrently with the new child process.
 * @param p_strProcessName_i This is the name that will be registered to the OS for this process
 * @param p_eSchedulingPolicy_i Scheduling Policy for this process
 *          FIFO          - A fixed priority scheduler in which the highest ready process runs until it
 *                          blocks or is preempted by a higher priority process.
 *          ROUND_ROBIN   - The same as FIFO, except processes at the same priority level time-slice.
 *          OTHER         - A general time sharing scheduler in which a process decays in priority if it
 *                          consumes too much processor before blocking. It reverts to its default priority
 *                          when it blocks. Should it fail to run over a 2 second period and it has decayed
 *                          then it's boosted one priority level up to a maximum of its default priority.
 * @param p_iPriority_i Priority for this process
 * @param p_lSpawnFlags_i Spawning flags. These are PosixBasedOS001 specific....
 * @return void
 */
void Process::CreateProcess(
            const SS_String& p_strFile_i,      // Path and Filename of executable to create process for
            const StringList& p_strlstArgv_i,    // List of ARGV values for new process
            const eProcessLoadMode p_eMode_i,    // Mode to create and load new process
            const SS_String& p_strProcessName_i,  // This is the name that will be registered to the OS for this process
            const eProcessSchedulingPolicy p_eSchedulingPolicy_i,  // Scheduling Policy for this process
            const int p_iPriority_i,        // Priority for this process
                      const char* unix_user_name,
            const long p_lSpawnFlags_i        // Posix Spawning flags. These are PosixBasedOS001 specific....  // NOLINT (runtime/int)
) {
  //========================================================================================
  // Perform some idiot checking of the parameters that are passed in.
  // Check the priority level that is being set to make sure it is in bounds.
  // Then save off the calling parameters into the objects member variables.
  // Also check the number of valid calling (argv[]) parameters passed in
  //========================================================================================
  m_iErrorCode = 0;   // Initialize to NO ERROR
  m_iReturnCode = 0;  // Initialize value (0 usually means good in unix/PosixBasedOS001)

  //
  // Process filename os provided
  //
  if (p_strFile_i.empty()) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }

  //
  // Save off process calling arguments
  //
  m_eProcessLoadMode = p_eMode_i;

  m_strFile = p_strFile_i;

  if (!p_strProcessName_i.empty())
    m_strProcessName = p_strProcessName_i;

  //
  // Copy Argument List...
  //
  SetCallingArgumentList(p_strlstArgv_i);

  //
  // Valid number of calling arguments
  //
  int iNumberElements = m_strlstArgv.size();
  if (iNumberElements > (iProcess_MAXIMUM_NUMBER_OF_PROCESS_ARGUMENTS - 3)) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }


  //========================================================================================
  // Initialize to the beginning of the provided argument list.
  // Allocate an array of buffer pointers that will be used for the ARGV calling parameters
  // for the specified process.
  // Set the ARGV[0] equal to the process name being created
  // Set the ARGV[1] equal to the process name to be registered with the OS.
  // If p_strProcessName_is NULL, do not set the cArgv[1] parameter
  // Populate the array that will hold the argument list for the new process we are creating.
  //========================================================================================
  //

  int iLoop = 1;
  char * cArgv[iProcess_MAXIMUM_NUMBER_OF_PROCESS_ARGUMENTS];

  cArgv[0] = basename(const_cast<char *>(p_strFile_i.c_str()));    // Set the executable filename

  // Go through the list of provided argv calling parameters to the CreateProcess
  // function, and copy the arguments to the ARGV[] calling argument which will be passed
  // into the process being created.
  StringListIter at = m_strlstArgv.begin();
  for (; at !=  m_strlstArgv.end(); at++, iLoop++) {
    cArgv[iLoop] = const_cast<char *>(at->c_str());
  }

  cArgv[iLoop] = NULL;

  try {
    CL_ProcessAttr_t        clAttr;
    CL_ProcessSchedPolicy_t clPolicy = CL_PROCESS_SCHED_POLICY_OTHER;
    int                     clPriority = 0;

    if (0 != CL_ProcessCreateAttrInit(&clAttr)) {
      throw "CL_ProcessCreateAttrInit()";
    }


    // In order to collect even the child processes of the service, all are group leaders.
    if (0 != CL_ProcessCreateAttrSetGroup(&clAttr, 1)) {
      throw "CL_ProcessCreateAttrInit()";
    }

    if (0 != CL_ProcessCreateAttrSetCpuAssign(&clAttr, m_cpu_assign)) {
      throw "CL_ProcessCreateAttrSetCpuAssign()";
    }

    switch (p_eSchedulingPolicy_i) {
      case FIFO:
        clPolicy =  CL_PROCESS_SCHED_POLICY_FIFO;
        break;
      case ROUND_ROBIN:
        clPolicy =  CL_PROCESS_SCHED_POLICY_RR;
        break;
      case OTHER:
        clPolicy =  CL_PROCESS_SCHED_POLICY_OTHER;
        break;
      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "p_eSchedulingPolicy_i = %d", p_eSchedulingPolicy_i);
        break;
    }

    switch (p_eSchedulingPolicy_i) {
      case FIFO:
      case ROUND_ROBIN:
        if ((1 > p_iPriority_i) || (p_iPriority_i > 99)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "p_eSchedulingPolicy_i = %d", p_eSchedulingPolicy_i);
        } else {
          clPriority = p_iPriority_i;
        }
        break;
      case OTHER:
      default:
        if ((-20 > p_iPriority_i) || (p_iPriority_i > 19)) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "p_eSchedulingPolicy_i = %d", p_eSchedulingPolicy_i);
        } else {
          clPriority = p_iPriority_i;
        }
        break;
    }

    if (0 != CL_ProcessCreateAttrSetSchedule(&clAttr, clPolicy, clPriority)) {
      throw "CL_ProcessCreateAttrSetSchedule()";
    }

    uid_t uid = 0;
    gid_t gid = 0;

    if (0 != getIdsFromUserName(unix_user_name, &uid, &gid)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "unexpected unix_user_name [%s]", unix_user_name);
      uid = geteuid();
      gid = getegid();
    }
    if (uid == 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "!! uid=root %s", m_strFile.c_str());
      uid = UINT_MAX;
    }
    if (0 != CL_ProcessCreateAttrSetUid(&clAttr, uid)) {
      throw "CL_ProcessCreateAttrSetUid()";
    }

    if (0 != CL_ProcessCreateAttrSetGid(&clAttr, gid)) {
      throw "CL_ProcessCreateAttrSetGid()";
    }

    if (0 != CL_ProcessCreateAttrSetDisableCloseFds(&clAttr)) {
      throw "CL_ProcessCreateAttrSetDisableCloseFds()";
    }
    char environment_string[2048] = {0};  // Size is provisional
    fprintf(stderr, "[%s](%d)Process Create Target : %s \n", __func__, __LINE__, m_strFile.c_str());

    CheckLdPreLoad(&m_strFile, environment_string);

    CreateProcess(&m_strFile, const_cast<char* const*>(cArgv), environment_string, &clAttr);
  }

  catch(const char *e) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "%s", e);
  }
}

/**
 * CreateProcess: This method will create a process with the executable provided and mode as a calling parameter.
 * The caller can also provide a list of arguments that will be provided to the executable at startup.
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information
 * will be maintained by this object.
 *
 * @param p_str_file Path and Filename of executable to create process for
 * @param c_argv use process arguments
 * @param environment_string Set LD_PRELOAD string
 * @param cl_attr CL_ProcessAttr_t
 * @return void
 */
void Process::CreateProcess(const SS_String *p_str_file, char* const*c_argv, char* environment_string,
                            const CL_ProcessAttr_t *cl_attr) {
  try {
    if ((p_str_file == NULL) || (c_argv == NULL) || (cl_attr == NULL)) {
      throw "CreateProcess() Invaild Param";
    }

    int process_id = -1;
    if (environment_string[0] == '\0') {
      // If there is no LD_PRELOAD setting, set envp to NULL
      process_id = CL_ProcessCreate(p_str_file->c_str(), c_argv, NULL, cl_attr);
    } else {
      // vector holding preferences
      std::vector<SS_String> *vec_environ = GetEnvironVector();

      // Set LD_PRELOAD string as the last element
      SS_String ld_preload_string = SS_String(environment_string);
      vec_environ->push_back(environment_string);

      // Number of acquired environment variables + 1(For terminal) memory allocation
      size_t env_num = sizeof(char*) * (vec_environ->size() + 1);
      char **p_environment = static_cast<char **>(malloc(env_num));
      memset(p_environment, 0x00, env_num);

      // Create environment variable list
      int i = 0;
      char **p_environment_tmp = p_environment;
      for (std::vector<SS_String>::iterator itr = vec_environ->begin(); itr != vec_environ->end(); itr++, i++) {
        p_environment_tmp[i] = static_cast<char *>(malloc(sizeof(char) * (itr->length() + 1)));

        snprintf(p_environment_tmp[i], itr->length() + 1, "%s", itr->c_str());
      }

      // Set envp for environment variable
      process_id = CL_ProcessCreate(p_str_file->c_str(), c_argv, p_environment, cl_attr);

      // Free memory
      i = 0;
      p_environment_tmp = p_environment;
      for (std::vector<SS_String>::iterator itr = vec_environ->begin(); itr != vec_environ->end(); itr++, i++) {
        free(p_environment_tmp[i]);
      }

      free(p_environment);
      delete(vec_environ);
    }

    if (process_id == -1) {
      throw "CL_ProcessCreate()";
    }

    m_tProcessId = process_id;
  }
  catch (const char *e) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "%s", e);
  }
}
/**
 * CreateProcess: This method will create a PosixBasedOS001 process with the executable provided and mode as a calling parameter. 
 * The caller can also provide a list of arguments that will be provided to the executable at startup. 
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information 
 * will be maintained by this object.
 *
 * @param p_strFile_i Path and Filename of executable to create process for
 * @param p_strProcessName This is the name that will be registered to the OS for this process 
 * @return void
 */
void Process::CreateProcess(const SS_String& p_strFile_i, const SS_String& p_strProcessName,
                            const char* unix_user_name, const long p_lSpawnFlags_i) {  // NOLINT (runtime/int)
  StringList strlstArgv;

  try {
    CreateProcess(p_strFile_i,
            strlstArgv,
            NOWAIT,
            p_strProcessName,
            FIFO,
            iProcess_DEFAULT_PROCESS_PRIORITY,
                        unix_user_name,
            p_lSpawnFlags_i);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}


/**
 * CreateProcess: This method will create a process with the executable provided and mode as a calling parameter.
 * The caller can also provide a list of arguments that will be provided to the executable at startup. 
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information 
 * will be maintained by this object.
 *
 *
 * @param p_strFile_i Path and Filename of executable to create process for
 * @param p_strProcessName This is the name that will be registered to the OS for this process
 * @param p_iPriority_i  Priority of process
 * @return void
 */
void Process::CreateProcess(const SS_String& p_strFile_i, const SS_String& p_strProcessName, const int p_iPriority_i,
                            const char* unix_user_name, const long p_lSpawnFlags_i) {  // NOLINT (runtime/int)
  StringList strlstArgv;

  try {
    eProcessSchedulingPolicy policy;

    if (0 < p_iPriority_i) {
       policy = FIFO;
    } else {
       policy = OTHER;
    }

    CreateProcess(p_strFile_i,
            strlstArgv,
            NOWAIT,
            p_strProcessName,
            policy,
            p_iPriority_i,
                        unix_user_name,
            p_lSpawnFlags_i);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}

/**
 * CreateProcess: This method will create a process with the executable provided and mode as a calling parameter.
 * The caller can also provide a list of arguments that will be provided to the executable at startup. 
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information 
 * will be maintained by this object.
 *
 * @param p_strFile_i Path and Filename of executable to create process for
 * @param p_lSpawnFlags_i Spawning flags. These are PosixBasedOS001 specific.
 * @return void
 */
void  Process::CreateProcess(const SS_String& p_strFile_i, const char* unix_user_name, const long p_lSpawnFlags_i) {  // NOLINT (runtime/int)
  StringList strlstArgv;

  try {
    CreateProcess(p_strFile_i,
            strlstArgv,
            NOWAIT,
            basename(const_cast<char *>(p_strFile_i.c_str())),
            FIFO,
            iProcess_DEFAULT_PROCESS_PRIORITY,
                        unix_user_name,
            p_lSpawnFlags_i);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}



/**
 * CreateProcess: This method will create a  process with the executable provided and mode as a calling parameter.
 * The caller can also provide a list of arguments that will be provided to the executable at startup. 
 * The calling p_strProcessName parameter is a textual name that will be
 * associated with the newly created process by the OS. The process state information 
 * will be maintained by this object.
 *
 * @param p_strFile_i Path and Filename of executable to create process for
 * @param p_strProcessName This is the name that will be registered to the OS for this process
 * @param p_strlstArgv_i  List of ARGV values for new process
 * @param p_lSpawnFlags_i Spawning flags. These are PosixBasedOS001 specific.
 * @return void
 */
void Process::CreateProcess(const SS_String& p_strFile_i, const SS_String& p_strProcessName_i,
                            const StringList& p_strlstArgv_i, const char* unix_user_name, const long p_lSpawnFlags_i) {  // NOLINT (runtime/int)
  try {
    CreateProcess(p_strFile_i,
            p_strlstArgv_i,
            NOWAIT,
            p_strProcessName_i,
            FIFO,
            iProcess_DEFAULT_PROCESS_PRIORITY,
                        unix_user_name,
            p_lSpawnFlags_i);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}


void Process::CreateProcess(const SS_String& p_strFile_i, const SS_String& p_strProcessName_i, const int p_iPriority_i,
                            const StringList& p_strlstArgv_i, const char* unix_user_name, const long p_lSpawnFlags_i) {  // NOLINT (runtime/int)
  try {
    eProcessSchedulingPolicy policy;

    if (0 < p_iPriority_i) {
       policy = FIFO;
    } else {
       policy = OTHER;
    }

    CreateProcess(p_strFile_i,
            p_strlstArgv_i,
            NOWAIT,
            p_strProcessName_i,
            policy,
            p_iPriority_i,
                        unix_user_name,
            p_lSpawnFlags_i);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}


/**
 * CreateProcessWait: This method will create a process with the executable provided.
 * The process state information will be maintained by this object.
 *
 * @param p_strFile_i Path and Filename of executable to create process for 
 * @return void
 */
void Process::CreateProcessWait(const SS_String& p_strFile_i) {
  StringList strlstArgv;

  try {
    CreateProcess(p_strFile_i,
            strlstArgv,
            WAIT,
            basename(const_cast<char *>(p_strFile_i.c_str())),
            FIFO,
            iProcess_DEFAULT_PROCESS_PRIORITY,
            iProcess_DEFAULT_PROCESS_USER_NAME,
            iProcess_DEFAULT_PROCESS_FLAGS);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}

/**
 * CreateProcessWait: This method will create a PosixBasedOS001 process with the executable provided.
 * The process state information will be maintained by this object.
 *
 *
 * @param p_strFile_i Path and Filename of executable to create process for 
 * @param p_strlstArguments_i List of process calling arguments
 * @return void
 */
void Process::CreateProcessWait(const SS_String& p_strFile_i, const StringList& p_strlstArguments_i) {
  try {
    CreateProcess(p_strFile_i,
            p_strlstArguments_i,
            WAIT,
            basename(const_cast<char *>(p_strFile_i.c_str())),
            FIFO,
            iProcess_DEFAULT_PROCESS_PRIORITY,
            iProcess_DEFAULT_PROCESS_USER_NAME,
            iProcess_DEFAULT_PROCESS_FLAGS);
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}



/**
 * KillProcess: This method will delete the process represented by this object. All variables associated
 * with this object will be initialized to a know value.
 *
 * @param  
 * @return void
 */
void Process::KillProcess(int signal) {
  //=====================================================================================
  // Intialize the objects m_iErrorCode member variable to 0 (no error).
  // Then try to delete the process that this object represents.
  m_iErrorCode = 0;

  if (DoesProcessExist()) {
    if (-1 == killpg(m_tProcessId, signal)) {
      return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
    }
  }

  //
  // If no errors, clear out any process specific member variables for this object
  //
  m_tProcessId = -1;
  m_strProcessName = "";
}

/**
 * SetSchedulingPolicy: This method will change the scheduling policy for the process this
 * object represents. 
 *
 * @param p_eSchedulingPolicy_i Scheduling policy 
 * @return void
 */
void Process::SetSchedulingPolicy(
          const eProcessSchedulingPolicy p_eSchedulingPolicy_i  // Scheduling policy
) {
  //=======================================================================================
  // Attempt to change the scheduling policy for the process that this object
  // represents. If the change fails, restore the previous settings.
  //
  m_iErrorCode = 0;
  struct sched_param cur_sch_params;
  sched_getparam(m_tProcessId, &cur_sch_params);
  if (0 != sched_setscheduler(m_tProcessId, ConvertToPosixBasedOS001SchedularPolicy(p_eSchedulingPolicy_i),
                              &cur_sch_params)) {
    m_iErrorCode = errno;
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}

/**
 * SetPriority: This method will change the priority for the process this
 * object represents. 
 *
 * @param p_iPriority_i Scheduling Policy for this process
 * @return void
 */
void Process::SetPriority(const int p_iPriority_i) {
  //=======================================================================================
  // Attempt to change the priority for the process that this object
  // represents. If the change fails, restore the previous settings.
  //
  m_iErrorCode = 0;
  struct sched_param cur_sch_params;
  sched_getparam(m_tProcessId, &cur_sch_params);
  cur_sch_params.sched_priority = p_iPriority_i;

  if (-1 == sched_setparam(m_tProcessId, &cur_sch_params)) {
    m_iErrorCode = errno;
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}


/**
 * IncreasePriorityByOne: This method will increase the priority for the process this
 * object represents by one.
 *
 * @param  
 * @return void
 */
void Process::IncreasePriorityByOne(void) {
  //================================================================================
  // Retrieve the current priority of the process. Check to see if already at max.
  // If so just return. Otherwise increase by one and set the priority...
  //
  try {
    int iCurrentPriority = GetPriority();
    if (iCurrentPriority < iProcess_MAXIMUM_PROCESS_PRIORITY) {
      SetPriority(iCurrentPriority + 1);
    }
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}


/**
 * DecreasePriorityByOne: This method will decrease the priority for the process this
 * object represents by one.
 *
 *
 * @return void
 */
void Process::DecreasePriorityByOne(void) {
  //================================================================================
  // Retrieve the current priority of the process. Check to see if already at minimum.
  // If so just return. Otherwise decrease by one and set the priority...
  //
  try {
    int iCurrentPriority = GetPriority();
    if (iCurrentPriority > 1) {
      SetPriority(iCurrentPriority - 1);
    }
  }
  catch (...) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }
}





////////////////////////////////////////////////////////////////////////////////////////////////////
// ConvertToPosixBasedOS001SchedularPolicy
//
// This method will return to the caller the equivalent PosixBasedOS001 schedular policy for the process
// that this object represents
//
//
// Calling Arguments:
//      NONE
//
// Return Argument:
//  FIFO,      // A fixed priority scheduler in which the highest ready process runs until it
//          // blocks or is preempted by a higher priority process.
//  ROUND_ROBIN,  // The same as FIFO, except processes at the same priority level time-slice.
//  OTHER      // A general time sharing scheduler in which a process decays in priority if it
//          // consumes too much processor before blocking. It reverts to its default priority
//          // when it blocks. Should it fail to run over a 2 second period and it has decayed
//          // then it's boosted one priority level up to a maximum of its default priority.
//
int const Process::ConvertToPosixBasedOS001SchedularPolicy(const eProcessSchedulingPolicy p_eSchedulingPolicy_i) {
  int iReturnValue = SCHED_RR;  // Default is RR
  switch (p_eSchedulingPolicy_i) {
    case FIFO:
    {
      iReturnValue = SCHED_FIFO;
      break;
    }

    case ROUND_ROBIN:
    {
      iReturnValue = SCHED_RR;
      break;
    }

    case OTHER:
    {
      iReturnValue = SCHED_OTHER;
      break;
    }
  }

  return iReturnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ConvertFromPosixBasedOS001SchedularPolicy
//
// This method will return to the caller the eProcessSchedulingPolicy based on the PosixBasedOS001 schedular
// policy for the process that this object represents
//
//
// Calling Arguments:
//      PosixBasedOS001 Scheduling Policy
//
// Return Argument:
//  FIFO,      // A fixed priority scheduler in which the highest ready process runs until it
//          // blocks or is preempted by a higher priority process.
//  ROUND_ROBIN,  // The same as FIFO, except processes at the same priority level time-slice.
//  OTHER      // A general time sharing scheduler in which a process decays in priority if it
//          // consumes too much processor before blocking. It reverts to its default priority
//          // when it blocks. Should it fail to run over a 2 second period and it has decayed
//          // then it's boosted one priority level up to a maximum of its default priority.
//
Process::eProcessSchedulingPolicy const Process::ConvertFromPosixBasedOS001SchedularPolicy
                                                 (const int p_iPosixBasedOS001chedulingPolicy_i) {
  eProcessSchedulingPolicy ePolicy = ROUND_ROBIN;  // Default is RR
  switch (p_iPosixBasedOS001chedulingPolicy_i) {
    case SCHED_FIFO:
    {
      ePolicy = FIFO;
      break;
    }

    case SCHED_RR:
    {
      ePolicy = ROUND_ROBIN;
      break;
    }
    case SCHED_OTHER:
    {
      ePolicy = OTHER;
      break;
    }
  }  // Switch

  return ePolicy;
}


/**
 * DoesProcessExist: This method will return a BOOLean indicating whether this 
 * process exists.
 *
 * @return BOOL TRUE  - Process Exists, FALSE - Process does not exist
 */
BOOL Process::DoesProcessExist(void) {
  struct sched_param cur_sch_params;
  if (-1 >=  sched_getparam(m_tProcessId, &cur_sch_params)) {  // or the segment data
    return FALSE;
  }

  return TRUE;
}



/**
 * SetProcessName: This method will set this objects process name member variable to the 
 * provided string value.
 *
 * @param p_strProcessName_i Process Name to set the m_strProcessName member variable to
 * @return void
 */
void Process::SetProcessName(const SS_String& p_strProcessName_i) {
  //
  // Idiot checking
  //
  assert(!p_strProcessName_i.empty());

  m_strProcessName = p_strProcessName_i;
}

/**
 * GetSchedulingPolicy: This method will return to the caller the 
 * currently configured process scheduling policy.
 *
 * @return Process::eProcessSchedulingPolicy const
 */
Process::eProcessSchedulingPolicy const Process::GetSchedulingPolicy(void) {
  int policy = 0;
  if (-1 == (policy = sched_getscheduler(m_tProcessId))) {
    m_iErrorCode = errno;
    return ROUND_ROBIN;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }

  return (ConvertFromPosixBasedOS001SchedularPolicy (policy));
}



/**
 * GetPriority: This method will return to the caller the currently configured
 * process priority.
 *
 * @return int const
 */
int const Process::GetPriority(void) {
  struct sched_param cur_sch_params;
  if (-1 >=  sched_getparam(m_tProcessId, &cur_sch_params)) {
    m_iErrorCode = errno;
    return -1;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }

  return (cur_sch_params.sched_priority);
}



/**
 * SetCallingArgumentList: This method will set the calling argument list 
 * that this object represents.
 *
 * @param p_pcArgv_i Pointer to NULL terminated argument list.
 * @param p_iArgc_i Number of parameters
 * @return void
 */
void Process::SetCallingArgumentList(const char *p_pcArgv_i[], const int p_iArgc_i) {
  ////////////////////////////////////////////////////////////////////////
  // Set the executable filename first. This is always the 1st argument
  // in the argument list. Then set the argument list for the process
  // which is held in m_strlstArgv.
  //
  if (p_iArgc_i > (iProcess_MAXIMUM_NUMBER_OF_PROCESS_ARGUMENTS - 3)) {
    return;  // PAD THIS IS BAD!!! TODO: Change this to something meaningful
  }

  //
  // First make sure that the argument list is empty.
  //
  if (!m_strlstArgv.empty()) {
    m_strlstArgv.clear();      // Remove all elements from the current list
  }  // Process Argument list is NOT empty

  //
  // Once empty, put the new arguments into the list
  //
  StringListIter at = m_strlstArgv.begin();
  int iLoop;
  for (iLoop = 0; iLoop < p_iArgc_i; iLoop ++) {
    at = m_strlstArgv.insert(at, p_pcArgv_i[iLoop]);
  }
}


/**
 * SetCallingArgumentList: This method will set the calling argument 
 * list that this object represents.
 *
 * @param p_strlstParameters_i List of parameters 
 * @return void
 */
void Process::SetCallingArgumentList(const StringList& p_strlstParameters_i) {
  if (p_strlstParameters_i.size()) {
    m_strlstArgv.clear();  // Empty the current list.
    m_strlstArgv = p_strlstParameters_i;
  }
}

/**
 * Search if libTestFwCommon.so is dynamically linked.
 *
 * @param process_path Search target binary path
 * @return bool
 */
static bool CheckLinkedTestfwLibrary(SS_String *process_path) {
  if (NULL == process_path) {
    fprintf(stderr, "[%s](%d)Invaild Param.\n", __func__, __LINE__);
    return false;
  }

  ELFIO::elfio reader;
  if (!reader.load(process_path->c_str())) {
    fprintf(stderr, "[%s](%d)%s is not ELF!\n", __func__, __LINE__, process_path->c_str());
    return false;
  }

  ELFIO::Elf_Half n = reader.sections.size();
  for ( ELFIO::Elf_Half i = 0; i < n; ++i ) {
    ELFIO::section* sec = reader.sections[i];
    if ( SHT_DYNAMIC == sec->get_type() ) {
      ELFIO::dynamic_section_accessor dynamic(reader, sec);

      ELFIO::Elf_Xword dynamic_num = dynamic.get_entries_num();
      if ( dynamic_num > 0 ) {
        for ( ELFIO::Elf_Xword i = 0; i < dynamic_num; ++i ) {
          ELFIO::Elf_Xword   dynamic_tag   = 0;
          ELFIO::Elf_Xword   dynamic_value = 0;
          SS_String dynamic_str;
          dynamic.get_entry(i, dynamic_tag, dynamic_value, dynamic_str);

          // Search if the acquired dynamic section string contains libTestFwCommon.so
          if (SS_String::npos != dynamic_str.find("libTestFwCommon.so")) {
            fprintf(stderr, "[%s](%d)libTestFwCommon is linked.\n", __func__, __LINE__);
            return true;
          }

          // Check for continuation of dynamic section element
          if ( DT_NULL == dynamic_tag ) {
            break;
          }
        }
      } else {
        // If dynamic section is not found
        fprintf(stderr, "[%s](%d)dynamic symbol is not find.\n", __func__, __LINE__);
      }
    }
  }

  fprintf(stderr, "[%s](%d)libTestFwCommon is not find. \n", __func__, __LINE__);
  return false;
}

/**
 * This method is a private method to be called from CreateProcess func.
 * Check environment variable MOCK_LIBRARY where Mock Library Name to set in LD_PRELOAD is set
 * and output character string to set in LD_PRELOAD if there is setting.
 *
 * @param process_path,environment_string
 * @return void
 */
void Process::CheckLdPreLoad(SS_String *process_path, char *environment_string) {
  if ((process_path == NULL || environment_string == NULL)) {
    fprintf(stderr, "[%s](%d)Invaild Param.\n", __func__, __LINE__);
    return;
  }

  // Check if environment variable MOCK_LIBRARY for LD_PRELOAD setting is set
  char *value_mock_library = getenv("MOCK_LIBRARY");

  if (value_mock_library != NULL) {
    // When MOCK_LIBRARY is set
    fprintf(stderr, "[%s](%d)MOCK_LIBRARY = %s \n", __func__, __LINE__, value_mock_library);

    // Check whether the process to be started is a core unit or TestFW, and execute LD_PRELOAD if libTestFwCommon.so is linked
    if (CheckLinkedTestfwLibrary(process_path)) {
      SS_String key_value;
      SS_String key_ld_preload = "LD_PRELOAD=";

      // Create LD_PRELOAD setting string
      // LD_PRELOAD is enabled when the string is an envp argument in the form "LD_PRELOAD=hoge_mock.so hoge_mock.so ..."
      key_value = key_ld_preload + value_mock_library;
      strncpy(environment_string, key_value.c_str(), key_value.length());
      fprintf(stderr, "[%s](%d)Set envp: %s \n", __func__, __LINE__, environment_string);
      return;
    } else {
      // If the unit does not require LD_PRELOAD setting, set envairoment_string to NULL.
      fprintf(stderr, "[%s](%d)Core Unit is not setting LD_PRELOAD\n", __func__, __LINE__);
      return;
    }
  } else {
    // If it is not set to MOCK_LIBRARY, it is not necessary to set LD_PRELOAD, so set envairoment_string to NULL.
    fprintf(stderr, "[%s](%d)The MOCK_LIBRARY variable was unestablished.\n", __func__, __LINE__);
    return;
  }
}

/**
 * Acquire the character string of the environment variable and set it in String Vector
 * and return in a pointer of Vector.
 *
 * @param process_path,environment_string
 * @return vector<SS_String> pointer
 */
extern char ** environ;
std::vector<SS_String> *Process::GetEnvironVector(void) {
  // If LD_PRELOAD is set, copy existing environment settings and set to environment_pointer

  std::vector<SS_String>* vector_environ = new std::vector<SS_String>;

  char **environ_tmp = environ;

  // Insert environ_string into the vector table
  int i = 0;
  while (environ_tmp[i] != NULL) {
    SS_String str_env(environ_tmp[i]);
    vector_environ->push_back(str_env);
    i++;
  }

  return vector_environ;
}
// ----------------------------------------
