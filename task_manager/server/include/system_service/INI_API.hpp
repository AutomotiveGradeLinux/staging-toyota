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

/**
 * @file INI_API.hpp
 * @brief \~english This file provide api to operating INI
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup task_manager
 *  @ingroup system_service
 *  @{
 */

#ifndef _INI_API_HPP_
#define _INI_API_HPP_

#include <native_service/frameworkunified_types.h>
#include <system_service/ss_system_manager_if.h>
#include "system_service/Primary_common.h"

#define INI_SUCCESS                 0              /* Normal */
#define INI_FALSE                  -1              /* Failed */

#define INI_FD_MAX                  3

// Only the information required for event completion notification is left
// Gradual startup
#define INI_INITCOMP_NONE                   0x0000000000000000LLU   
#define INI_INITCOMP_ON_START               0x0000000000000001LLU   /* ON_START    */
#define INI_INITCOMP_NVM_ACCESS             0x0000000000000002LLU   /* NVM ACCESS  */

#define INI_INITCOMP_TEST0                  0x1000000000000000LLU   /* for test    */
#define INI_INITCOMP_TEST1                  0x2000000000000000LLU   /* for test    */
#define INI_INITCOMP_TEST2                  0x4000000000000000LLU   /* for test    */
#define INI_INITCOMP_TEST3                  0x8000000000000000LLU   /* for test    */

// Gradual termination
#define INI_TERMCOMP_NONE                   0x0000000000000000LLU   
#define INI_TERMCOMP_ACTIVITYMGR            0x0000000000000001LLU   /* Terminate ActivityManager  */
#define INI_TERMCOMP_RESIDENT               0x0000000000000002LLU   /* Terminate regident SVC     */
#define INI_TERMCOMP_TRANSIENT              0x0000000000000004LLU   /* Terminate non-regident SVC */

#define INI_TERMCOMP_TEST0                  0x1000000000000000LLU   /* for test    */
#define INI_TERMCOMP_TEST1                  0x2000000000000000LLU   /* for test    */
#define INI_TERMCOMP_TEST2                  0x4000000000000000LLU   /* for test    */
#define INI_TERMCOMP_TEST3                  0x8000000000000000LLU   /* for test    */



typedef struct _T_PRIM_PRM{
  PCSTR                      name;
  const PRIM_SHAREDATA_TBL*  shmTbl;
  const PRIM_EXFUNC_TBL*     wakeupExFuncTbl;
  const PRIM_EXFUNC_TBL*     downExFuncTbl;
  EFrameworkunifiedStatus (*onInit)(HANDLE hApp);
  EFrameworkunifiedStatus (*onDestory)(HANDLE hApp);
  EFrameworkunifiedStatus (*onDebugDump)(HANDLE hApp);
  EFrameworkunifiedStatus (*onTouch)(HANDLE hApp);
  EFrameworkunifiedStatus (*onLowMem)(HANDLE hApp);
  void* priv;
  _T_PRIM_PRM(){
    name            = NULL;
    shmTbl          = NULL;
    wakeupExFuncTbl = NULL;
    downExFuncTbl   = NULL;
    onInit          = NULL;
    onDestory       = NULL;
    onDebugDump     = NULL;
    onTouch         = NULL;
    onLowMem        = NULL;
    priv            = NULL;
  }
}T_PRIM_PRM;


typedef struct {
  BOOL      bIsRun;
  uint32_t  timeout;
}T_PRIM_MONITOR_PRM;

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_Main
/// \~english @par Summary:
///        Runs the main thread of primary library.
/// \~english @param  [in] p_prm
///        T_PRIM_PRM   - The pointer to the parameter table of primary library
/// \~english @param  [in] argc
///        int   -  The number of command parameters (The parameters passed to main functions must be set without processing.)
/// \~english @param  [in] argv
///        char* -  Command parameters (The parameters passed to main functions must be set without processing.)
/// \~english @par
///    - Shared memory creation table \n
///      In process running, register the shared memory to create.\n
///      This creates the shared memory to be registered in process running and releases it in process end.\n
///      This must terminate by adding {"\0"} into member shmName of table termination.\n
///      It is able to access to the shared memory to have been created by shm_open.\n
/// \~english @code
///         typedef struct {
///             char       shmName[32];                   /* Shared memory name           */
///             uint32_t   size;                          /* Shared memory size (BYTE) */
///         }PRIM_SHAREDATA_TBL;
///            @endcode
/// \~english @par
///    - Running extended function table / End extended function table \n
///      Registers callback functions to be called in process start and exit.\n
///      This must terminate by adding NULL into member func of table termination.\n
///      Parameters prm of callback functions are User private information and the pointer designated by prm is passed as input parameters [in].\n
///      Local phase numbers callback func by the designated phase numbers.\n
/// \~english @code
///         typedef struct {
///             void     (* func)( void* prm);             /* Callback functions for function extension           */
///             void*    prm;                              /* The pointer to the parameters of Callback function func  */
///             uint8_t  localStep;                        /* Local phase numbers                       */
///             uint8_t  rsv[3];                           /* Reserve for alignment               */
///         }PRIM_EXFUNC_TBL;
///            @endcode
/// \~english @par
///    - %Parameter table of primary library\n
///      For FrameworkunifiedOnInitialization/FrameworkunifiedOnDestroy/FrameworkunifiedOnDebugDump/TaskmanagerOnTouch/FrameworkunifiedOnLowMemory, check the corresponding API specification.\n
///      It is able to get User private data by INI_GetPrivate().\n
/// \~english @code
///         typedef struct _T_PRIM_PRM{
///             PCSTR                      name;            /* Application name                       */
///             const PRIM_SHAREDATA_TBL*  shmTbl;          /* The pointer to shared memory creation table        */
///             const PRIM_EXFUNC_TBL*     wakeupExFuncTbl; /* The pointer to running extended function table          */
///             const PRIM_EXFUNC_TBL*     downExFuncTbl;   /* The pointer to end extended function table          */
///             EFrameworkunifiedStatus (*onInit)(HANDLE hApp);          /* FrameworkunifiedOnInitialization callback function setting */
///             EFrameworkunifiedStatus (*onDestory)(HANDLE hApp);       /* FrameworkunifiedOnDestroy callback function setting        */
///             EFrameworkunifiedStatus (*onDebugDump)(HANDLE hApp);     /* FrameworkunifiedOnDebugDump Callback function setting     */
///             EFrameworkunifiedStatus (*onTouch)(HANDLE hApp);         /* TaskmanagerOnTouch callback function setting          */
///             EFrameworkunifiedStatus (*onLowMem)(HANDLE hApp);        /* FrameworkunifiedOnLowMemory callback function setting      */
///             void* priv;                                 /* User private data */
///         }T_PRIM_PRM;
///            @endcode
/// \~english @retval INI_SUCCESS Normal end
/// \~english @retval INI_FALSE  Abnormal end
/// \~english @par Preconditions:
///       - None
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The pointer to the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the shared memory creation table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the running extended function table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the end extended function table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnInitialization callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnDestroy callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnDebugDump callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of TaskmanagerOnTouch callback function inside the parameter table of primary library is NULL [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Method only
/// \~english @par Detail:
///       This executes the main processing of primary library. Functions do not return until all of end processing of process is completed. \n
///       Normal end returns INI_SUCCESS and abnormal end does INI_FALSE. \n
///       User process must return this function's returned values as the returned values of main function and end main function. \n
/// \~english @see  INI_Init
////////////////////////////////////////////////////////////////////////////////////
int     INI_Main(T_PRIM_PRM* p_prm,int argc,char* argv[]);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_ExitStart
/// \~english @par Summary:
///        Starts the end processing of process.
/// \~english @param  [in] rsv
///        void*   - The pointer to reservation parameters (NULL must be appointed)
/// \~english @retval None
/// \~english @par Preconditions:
///       - None
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Collects the resource which primary library got, does the end processing equal to local_step=all and end process. \n
///       (At the end processing in abnormality, process must be ended by not this function but exit(),_exit() function immediately) \n
///       When this API is called after this API is already called, do nothing to process.
/// \~english @par Supplemtent:
///       local_step is the information defined in default_wakeup.xml/default_shutdown.xml as the definition of running order / end order. \n
///       local_step defined in default_shutdown.xml has the following types.
///       - 1-9  : Designates local phase numbers (the phase numbers of end processing)
///       - last : This designates local phase numbers to be last. When there is the local phase processing not to be executed, this executes all.
///       - all  : The same as last.
/// \~english @see  INI_Main, INI_Init, INI_ExitDone
////////////////////////////////////////////////////////////////////////////////////
void    INI_ExitStart(void* rsv); // Termination process start

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_ExitDone
/// \~english @par Summary:
///        Notifies the completion of process end processing.
/// \~english @param  [in] status
///        int   -   Unused (This is for future extension. 0 must be designated.)
/// \~english @retval None
/// \~english @par Preconditions:
///       - None
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Notifies the completion of service end processing to TaskManager. \n
///       TaskManager ends the corresponding service by this notification. At the time, this callbacks FrameworkunifiedOnDestroy(). \n
/// \~english @see  INI_Main, INI_Init, INI_ExitStart
////////////////////////////////////////////////////////////////////////////////////
void    INI_ExitDone(int status);  // Termination process completed

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_GetPrivate
/// \~english @par Summary:
///        Gets private data.
/// \~english @param  None
/// \~english @retval void*  The pointer to user private data
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called and user private data must be set.
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Gets the private data designated at INI_Main/INI_Init from the inside of the context management information of the process inside primary library and returns it.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
void*   INI_GetPrivate();

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_GetHandle
/// \~english @par Summary:
///        Gets the application handle of NSFW.
/// \~english @param  None
/// \~english @retval void*(HANDLE)  The application handle of NSFW
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called and the application handle of NSFW must be generated.
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Gets the application handle of NSFW from the inside of the context management information of the process inside primary library and returns it.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
void*   INI_GetHandle();

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_SetMonitorTimeout
/// \~english @par Summary:
///        Requests the change for abnormality monitoring timeout time to Task Manager. \n
///        (This is used when main thread is not implemented at service side)
/// \~english @param  [in] timeout
///        uint32_t -  Abnormality monitoring timeout time (sec)(When 0 is designated, the service must be out of abnormality monitoring object.)
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @par Preconditions:
///       - Main thread must be run at INI_Main()
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Sets the abnormality monitoring timeout time designated by parameters inside the context management information of the process inside primary library and returns returned values and INI_SUCCESS. \n
///       Set timeout time becomes valid after the event waiting at the loop of main thread and this requests the change for service abnormality monitoring timeout time to TaskManager. In other words, this API can be used after running extended function Callback. \n
///       When process does not return to main thread past the timeout time designated by this function, TaskManager ends the service forcibly. When this function is not called, TaskManager treats the timeout of monitoring time as 50 seconds.
/// \~english @see  INI_Main
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_SetMonitorTimeout(uint32_t timeout);

// Event completion notification
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_StepForkComp
/// \~english @par Summary:
///        Notifies the completion of running event to TaskManager.
/// \~english @param  [in] compId
///        uint64_t   - The comparison ID for phase running (Designates INI_INITCOMP_[a-zA-Z_]+ to define at INI_API.hpp)
/// \~english @par
///        The ID descripted at INI_API.hpp
/// \~english @code
///        - INI_INITCOMP_NONE:         None
///        - INI_INITCOMP_ON_START:    For ON Start
///        - INI_INITCOMP_NVM_ACCESS:  For NVM access
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called.
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The sending of the completion notification of running event to TaskManager failed [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Sends the comparison ID for phase running as the completion notification of running event to TaskManager. If it succeeded, returns INI_SUCCESS and if it failed, does INI_FALSE. \n
///       TaskManager holds which completion notification of running event is notified and can add it to the transition conditions to next phase at phase running. \n
///       The way to add at TaskManager side depends on the definition of next_trans_condition element described in default_wakeup.xml as the definition of running order.
/// \~english @par Supplement:
///       next_trans_condition is the element to designate transition conditions to next phase. The transition to next phase is done when the following two conditions are met. \n
///       When the description of next_trans_condition element is omitted, transition is done only by condition 1. \n
///       1. All of the phase running request processing set by request element is completed. (The phase running request processing is executed by primary library asynchronously) \n
///       2. The completion notification of the event designated by next_trans_condition element is completed. \n
///       Waiting conditions for event completion notification INI_INITCOMP_[A-Z]+ stipulated at INI_API.hpp is set in cond property designated at next_trans_condition element. \n
///       AND condition can be set by being INI_INITCOMP_AAAA| INI_INITCOMP_BBBB when conditions are some. \n
///       Event completion notification can be issued by using INI_StepForkComp.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_StepForkComp(uint64_t compId);

// Event completion notification
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_AccOffComp
/// \~english @par Summary:
///        Notifies end event completion notification to TaskManager.
/// \~english @param  [in] compId
///        uint64_t   - The comparison ID for phase running (Designates INI_TERMCOMP_[a-zA-Z_]+ defined at INI_API.hpp)
/// \~english @par
///        The ID described at INI_API.hpp
/// \~english @code
///        - INI_TERMCOMP_NONE         :  None
///        - INI_TERMCOMP_ACTIVITYMGR  : For ActivityManager end
///        - INI_TERMCOMP_RESIDENT     : For resident service end
///        - INI_TERMCOMP_TRANSIENT    : For non-resident service end
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The sending of the completion notification of end event to TaskManager failed [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Sends the comparison ID for phase running as the completion notification of end event to TaskManager. If it succeeded, returns INI_SUCCESS and if it failed, return INI_FALSE. \n
///       TaskManager holds which completion notification of end event is notified and can add it to the transition conditions to next phase at phase end. \n
///       The way to add at TaskManager side depends on the definition of next_trans_condition element described in default_shutdown.xml as the definition of end order.
/// \~english @par Supplement:
///       next_trans_condition is the element to designate transition conditions to next phase. The transition to next phase is done when the following two conditions are met. \n
///       When the description of next_trans_condition element is omitted, transition is done only by condition 1. \n
///       1. All of the phase end request processing set by request element is completed. (The phase end request processing is executed by primary library asynchronously) \n
///       2. The completion notification of the event designated by next_trans_condition element is completed. Waiting conditions for event completion notification INI_INITCOMP_[A-Z]+ stipulated at INI_API.hpp is set in cond property designated at next_trans_condition element. \n
///       AND condition can be set by being INI_INITCOMP_AAAA| INI_INITCOMP_BBBB when conditions are some. \n
///       Event completion notification can be issued by using INI_ AccOffComp.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_AccOffComp(uint64_t compId);

// Get boot info
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_GetBootInfo
// \~english @par Summary:
///        Gets boot information.
/// \~english @param  [in] info
///        T_SS_SM_START_DataStructType *   - The pointer to boot information structure
/// \~english @par
///         T_SS_SM_START_DataStructType structure
/// \~english @code
///            typedef struct T_SS_SM_START_DataStruct{
///                EPWR_WAKEUP_FACTOR_TYPE   startupReason;    /* Startup reason                   */
///                BOOL                      isUserModeOn;     /* User mode ON/OFF         */
///                ESMDataResetModeInfo      dataResetMode;    /* Data reset mode       */
///                EPWR_SC_SECURITY_STATUS   securityStatus;   /* Security state           */
///                EPWR_SC_WAKEUP_TYPE       wakeupType;       /* Battery disconnection state         */
///                ESMDramBackupStatus       dramBackupStatus; /* Backup memory area state */
///                ESMResetStatus            resetStatus;      /* Reset state               */
///              UI_32                     resetCount;       /* Abnormal reboot number           */
///            } T_SS_SM_START_DataStructType;
///
///            Startup reason type:
///                - epswfIGN_ACC     : Ignition ACC (Currently, Fixed)
///            User mode ON/OFF:
///                - TRUE             : User mode ON     (Currently, Fixed)
///            Data reset mode:
///                - e_SS_SM_DATA_RESET_NONE: Not-initialized
///                - e_SS_SM_DATA_RESET_USER: User data initialization
///                - e_SS_SM_DATA_RESET_FACTORY: Factory initialization
///            Security state:
///                - epsssUNLOCK      : Unlock state
///                - epsssLOCK        : Lock state
///            Battery disconnection state:
///                - epsstWARMSTART   : Battery disconnection does not occur
///                - epsstCOLDSTART   : Battery disconnection occurs
///            Backup memory area state:
///                - e_SS_SM_DRAM_BACKUP_OK: Backup OK of Backup memory area
///                - e_SS_SM_DRAM_BACKUP_NG: Backup NG of Backup memory area
///            Reset state:
///                - e_SS_SM_RESET_STATUS_UNSET: Information unsetting(Not decided yet)
///                - e_SS_SM_RESET_STATUS_NONE : Normal
///                - e_SS_SM_RESET_STATUS_NG   : Abnormality occurred (Abnormal RESET or booting after service abnormal end)
///                - e_SS_SM_RESET_STATUS_IMMEDIATE : Immediate RESET occurred
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called. (%Process running must be completed)
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The pointer to boot information structure designated at parameters is NULL [INI_FALSE]
///       - Startup reason type inside the context management information of the process inside primary library is wrong (%Process running is not completed) [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Gets boot information which is managed inside the context management information of the process inside primary library.
///       If it succeeded, returns INI_SUCCESS and if it failed, does INI_FALSE. \n
///       This sets valid values as info values in FrameworkunifiedOnStart of TaskManager. In other words, this sets valid values after running ActivityManager.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_GetBootInfo(T_SS_SM_START_DataStructType *info);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_GetExtBootInfo
/// \~english @par Summary:
///        Gets boot extended information.
/// \~english @param  [in] info
///        T_SS_SM_START_ExtDataStructType *   - The pointer to boot extended information structure
/// \~english @par
///         T_SS_SM_START_ExtDataStructType structure
/// \~english @code
///            #define SS_SM_START_EXT_INFO_SIZE 64
///            #define SS_SM_START_EXT_BODY_SIZE ( sizeof(BOOL)
///                                                + sizeof(EMRelaunchStatus)
///                                                + sizeof(BOOL)
///                                                + sizeof(BOOL)
///                                              )
///            define SS_SM_START_EXT_RSV_SIZE  SS_SM_START_EXT_INFO_SIZE
///                                            - SS_SM_START_EXT_BODY_SIZE
///            typedef struct {
///                BOOL                      isProgUpdated;     /* Program update information       */
///                EMRelaunchStatus relaunchStatus;             /* Own-service Relaunch state */
///                BOOL             isMapUpdated;               /* Map update information               */
///                BOOL             isMapDiffUpdated;           /* Map difference update information         */
///                uint8_t          reserved[SS_SM_START_EXT_RSV_SIZE]; /* Reserve area     */
///            } T_SS_SM_START_ExtDataStructType;
///
///            Program update information
///                - TRUE  : Program update
///                - FALSE : No Program update
///            Own-service Relaunch state:
///                - e_SS_SM_RELAUNCH_STATUS_NONE : Relaunch does not occur (Initial boot)
///                - e_SS_SM_RELAUNCH_STATUS_SAFE : Normal Relaunch
///                - e_SS_SM_RELAUNCH_STATUS_ERR  : Abnormal Relaunch
///            Map update information:
///                - TRUE  : Map update
///                - FALSE : No Map update
///            Map difference update information
///                - TRUE  : Map difference update
///                - FALSE : No Map difference update
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - INI_Main or INI_Init must be called. (%Process running must be completed.)
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The pointer to boot information structure designated at parameters is NULL [INI_FALSE]
///       - Startup reason type inside the context management information of the process inside primary library is wrong (%Process running is not completed) [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Gets boot extended information which is managed inside the context management information of the process inside primary library.
///       If it succeeded, returns INI_SUCCESS and if it failed, does INI_FALSE \n
///       This sets valid values as info values in FrameworkunifiedOnStart of TaskManager. In other words, this sets valid values after running ActivityManager.
///       Because TaskManager does not have Relaunch function, always sets e_SS_SM_RELAUNCH_STATUS_NONE in relaunchStatus.
/// \~english @see  INI_Main, INI_Init
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_GetExtBootInfo(T_SS_SM_START_ExtDataStructType *info);

//withoutMainLoop
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_Init
/// \~english @par Summary:
///        Initializes primary library. \n
///        (This is used when implementing main thread at service side)
/// \~english @param  [in] p_prm
///        T_PRIM_PRM   - The pointer to parameter table of primary library
/// \~english @param  [in] argc
///        int   -  The number of command parameters (The parameters passed to main functions must be set without processing)
/// \~english @param  [in] argv
///        char* -  Command parameters (The parameters passed to main functions must be set without processing)
/// \~english @param  [out] fdNum
///        int * -  The number of element stored in fdlist (INI_FD_MAX:3)
/// \~english @param  [out] fdlist[INI_FD_MAX]
///        int -  The array of FD (File Descriptor) for event reception which primary library waits
/// \~english @par
///    - Shared memory creation table \n
///      In process running, register the shared memory to create. \n
///      This creates the shared memory to be registered in process running and releases it in process end. \n
///      This must terminate by adding {"\0"} into member shmName of table termination. \n
///      It is able to access to the shared memory to have been created by shm_open. \n
/// \~english @code
///         typedef struct {
///             char       shmName[32];                   /* Shared memory name           */
///             uint32_t   size;                          /* Shared memory size (BYTE) */
///         }PRIM_SHAREDATA_TBL;
///            @endcode
/// \~english @par
///    - Running extended function table / End extended function table \n
///      Registers callback functions to be called in process start and exit. \n
///      This must terminate by adding NULL into member func of table termination. \n
///      Parameters prm of callback functions are User private information and the pointer designated by prm is passed as input parameters [in]. \n
///      Local phase numbers callback func by the designated phase numbers. \n
/// \~english @code
///         typedef struct {
///             void     (* func)( void* prm);             /* Callback functions for function extension           */
///             void*    prm;                              /* The pointer to the parameters of Callback function func  */
///             uint8_t  localStep;                        /* Local phase numbers                       */
///             uint8_t  rsv[3];                           /* Reserve for alignment               */
///         }PRIM_EXFUNC_TBL;
///            @endcode
/// \~english @par
///    - %Parameter table of primary library \n
///      For FrameworkunifiedOnInitialization/FrameworkunifiedOnDestroy/FrameworkunifiedOnDebugDump/TaskmanagerOnTouch/FrameworkunifiedOnLowMemory, check the corresponding API specification. \n
///      It is able to get User private data by INI_GetPrivate(). \n
/// \~english @code
///         typedef struct _T_PRIM_PRM{
///             PCSTR                      name;            /* Application name                        */
///             const PRIM_SHAREDATA_TBL*  shmTbl;          /* The pointer to shared memory creation table        */
///             const PRIM_EXFUNC_TBL*     wakeupExFuncTbl; /* The pointer to running extended function table          */
///             const PRIM_EXFUNC_TBL*     downExFuncTbl;   /* The pointer to end extended function table          */
///             EFrameworkunifiedStatus (*onInit)(HANDLE hApp);          /* FrameworkunifiedOnInitialization callback function setting */
///             EFrameworkunifiedStatus (*onDestory)(HANDLE hApp);       /* FrameworkunifiedOnDestroy callback function setting        */
///             EFrameworkunifiedStatus (*onDebugDump)(HANDLE hApp);     /* FrameworkunifiedOnDebugDump Callback function setting      */
///             EFrameworkunifiedStatus (*onTouch)(HANDLE hApp);         /* TaskmanagerOnTouch callback function setting          */
///             EFrameworkunifiedStatus (*onLowMem)(HANDLE hApp);        /* FrameworkunifiedOnLowMemory callback function setting      */
///             void* priv;                                 /* User private data */
///         }T_PRIM_PRM;
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - None
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The pointer to the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the shared memory creation table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the running extended function table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The pointer to the end extended function table inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnInitialization callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnDestroy callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of FrameworkunifiedOnDebugDump callback function inside the parameter table of primary library is NULL [INI_FALSE]
///       - The setting of TaskmanagerOnTouch callback function inside the parameter table of primary library is NULL [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Initializes primary thread. If it succeeded, returns INI_SUCCESS and if it failed, does INI_FALSE. \n
///       When implementing main thread at service, this function must be called in running main thread. \n
///       Waits fd of fdlist gotten by functions by select and when there is change in fd, service must execute event handler of primary by calling INI_Handler().
/// \~english @see  INI_Main
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_Init(T_PRIM_PRM* p_prm,int argc ,char* argv[],int *fdNum,int fdlist[INI_FD_MAX]);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_Handler
/// \~english @par Summary:
///        Processes the reception event of primary library. \n
///        (This is used when implementing main thread at service side)
/// \~english @param  [in] p_fds
///        fd_set* -  fd set varied and detected by select()
/// \~english @retval TRUE Service running (Continuing)
/// \~english @retval FALSE  The process end processing of primary library is completed
/// \~english @par Preconditions:
///       - Main thread must be run at INI_Init()
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Access is not from main thread context  [FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       When FD which primary library should process in fd set designated by p_fds is included, processes event processing of primary library. \n
///       When implementing main thread by service, it is necessary to execute the event reception processing of primary library by this function. \n
///       Returns TRUE during service running and does FALSE when the process end processing of primary library was completed. When FALSE is returned, service must call
///       INI_Term() and end itself.
/// \~english @see  INI_Init
////////////////////////////////////////////////////////////////////////////////////
BOOL INI_Handler(fd_set* p_fds);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_Term
/// \~english @par Summary:
///        Releases the resource of primary library. \n
///        (This is used when implementing main thread at service side)
/// \~english @param  None
/// \~english @retval None
/// \~english @par Preconditions:
///       - Main thread must be run at INI_Init()
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - None
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Releases the resource of primary library. \n
///       When implementing main thread at service, this function must release the resource of primary library in service end.
/// \~english @see  INI_Init, INI_Handler
////////////////////////////////////////////////////////////////////////////////////
void INI_Term(void);

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup INI_SetMonitorState
/// \~english @par Summary:
///        Sets abnormality monitoring state. \n
///        (This is used when implementing main thread at service side)
/// \~english @param  [in] p_prm
///        T_PRIM_MONITOR_PRM   - The pointer to the parameters structure for monitoring
/// \~english @par
///         T_PRIM_MONITOR_PRM structure
/// \~english @code
///         typedef struct {
///             BOOL        bIsRun;     /* Monitoring state (TRUE:RUN  FALSE:SLEEP) */
///             uint32_t    timeout;    /* Monitoring timeout time (sec)        */
///         }PRIM_SHAREDATA_TBL;
///            @endcode
/// \~english @retval INI_SUCCESS Succeeded
/// \~english @retval INI_FALSE  Failed
/// \~english @par Preconditions:
///       - Main thread must be run at INI_ Init ()
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - The pointer(p_prm) to the parameters structure for monitoring designated by parameters is NULL [INI_FALSE]
/// \~english @par Classification:
///       Public
/// \~english @par Type
///       Sync only
/// \~english @par Detail:
///       Sets abnormality monitoring for TaskManager. If it succeeded, returns INI_SUCCESS and if it failed, does INI_FALSE \n
///       When bIsRun=FALSE is not set until time designated by timeout passes after setting bIsRun=TRUE, TaskManager decides that the service falls into abnormal state and ends forcibly. \n
///        When 0 is designated at monitoring timeout time(timeout), the service must be out of abnormality monitoring object.
///       When monitoring state (bIsRun) is FALSE, monitoring state becomes invalid. \n
///       This API can be used after running extended function CALLBACK.
/// \~english @see  INI_Init
////////////////////////////////////////////////////////////////////////////////////
int32_t INI_SetMonitorState(T_PRIM_MONITOR_PRM *p_prm);


void _INI_DEBUGDUMP(BOOL bIsNeedSvcName, PCSTR f_cFormat, ...);

#define INI_DEBUGDUMP(args...)     _INI_DEBUGDUMP(TRUE,  ## args)
#define INI_DEBUGDUMP_RAW(args...) _INI_DEBUGDUMP(FALSE, ## args)

// Callback functions
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup TaskmanagerOnTouch
/// \~english @par Summary:
///        Callback to notify process use
/// \~english @param  [in] hApp
///        HANDLE   - Application handle
/// \~english @retval eFrameworkunifiedStatusOK Succeeded
/// \~english @retval Other-than-eFrameworkunifiedStatusOK  Failed (Error type depends on the implementation at user side)
/// \~english @par Preconditions:
///       - Must set inside running information structure in calling INI_Main or  INI_Init
/// \~english @par Changes of the internal state:
///       - The internal state is not changed
/// \~english @par Causes of failures:
///       - Implementation specification of callback depends on the implementation (design) of an application. For this, error type on failure conditions must be considered at an application side.
/// \~english @par Detail:
///       Service is call backed just before using.  \n
///       INI_ExitStart must not be issued for 30 seconds after service called this function. \n
///       Timeout must be implemented such as counting up by using interval timer so that INI_ExitStart is not issued by timeout after this API was called. \n
///       This function cannot call the API (TSKM_Xxxx) of TaskManager. (If this calls, deadlock occurs)
/// \~english @par Classification:
///          Public
/// \~english @see  None
////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus TaskmanagerOnTouch(HANDLE hApp);

#ifdef __cplusplus
}
#endif


#endif //_INI_API_HPP_

/** @}*/ // end of TaskManager
/** @}*/ // end of SystemService
/** @}*/ // end of BaseSystem
