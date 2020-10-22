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
 * @file tskm_svc.h
 * @brief \~english define of tskm
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
#ifndef TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_SVC_H_
#define TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_SVC_H_


#include <unistd.h>
#include <sys/types.h>
#include "system_service/tskm_local_type.h"


#define TSKM_SVC_WAIT_REQ_MAX  (8)      // Number of queued requests

// Service type
typedef enum {
  TSKM_SVC_TYPE_NATIVE,   //
  TSKM_SVC_TYPE_UNKNONW,  //
} TSKM_SVC_TYPE_t;

// Scheduling policy
typedef enum {
  TSKM_SVC_POLICY_TSS,   //
  TSKM_SVC_POLICY_RR,    //
  TSKM_SVC_POLICY_FIFO,  //
} TSKM_SVC_POLICY_t;

// Life cycle
typedef enum {
  TSKM_SVC_LC_ALWAYS,               // Resident
  TSKM_SVC_LC_ALWAYS_RECOVERABLE,   // Resident(Recoverable)
  TSKM_SVC_LC_DYNAMIC               // Non-resident
} TSKM_SVC_LC_t;

// CPU allocation: This defined value should match the specification of the second argument of CL_ProcessCreateAttrSetCpuAssign

typedef enum {
  TSKM_SVC_ASSIGN_CPU_AUTO = 0,  // Automatic allocation
  TSKM_SVC_ASSIGN_CPU_0 = 1,     // Fixed to CPU0
  TSKM_SVC_ASSIGN_CPU_1 = 2,     // Fixed to CPU1
} TSKM_SVC_ASSIGN_CPU_t;

// Service attributes (given by XML)
typedef struct {
  TSKM_SVCID_t svcId;        //
  const char* name;         //
  const char* path;         //
  char** args;         //
  TSKM_SVC_TYPE_t type;         //
  uint32_t prio;         //
  TSKM_SVC_POLICY_t policy;       //
  TSKM_SVC_LC_t lifeCycle;    //
  uint32_t retryCnt;     //
  TSKM_SVC_ASSIGN_CPU_t cpuAssign;    //
  const char* user;         //
  uint32_t runtimeLimit;  //
  uint32_t cpuLimit;     //
  uint32_t memLimit;     //
  TSKM_BOOL_t shotdownWait;  //
  uint32_t subgidNum;    //
  gid_t *subgidList;  //
} TSKM_SVC_ATTR_t;

// Service status
typedef enum {
  TSKM_SVC_DORMANT,      // During dormant (Not exec)
  TSKM_SVC_WAITCONNECT,  // Wait for connection
  TSKM_SVC_WAKEUP,       // Start in progress
  TSKM_SVC_RUNNING,      // Running (Startup completed)
  TSKM_SVC_DOWN,         // Termination in progress
  TSKM_SVC_FINDOWN,      // Termination completed
  TSKM_SVC_DISABLE,      // Prohibitting startup  (exec prohibited)
} TSKM_SVC_STATE_t;

// Service
typedef struct {                                                       // Life cycle
  TSKM_SVC_ATTR_t *attr;      // Service Attributes            ALL
  TSKM_SVC_STATE_t state;     // Service State                 ALL
  int iFd;                    // inotifyFd for touch           ALL
  pid_t pid;                  // PID of service                REQ_WAKEUP - FIN_DOWN
  int connFd;                 // service communication socket  DO_WAKEUP  - FIN_DOWN
  T_SS_SM_START_DataStructType bootInfo;        // Boot info                     REQ_WAKEUP - FIN_DOWN
  T_SS_SM_START_ExtDataStructType extBootInfo;  // Extended boot info            REQ_WAKEUP - FIN_DOWN

  TSKM_BOOL_t isShmDone;      // Shared memory initialization completed
  TSKM_BOOL_t isStepDone;     // Gradual startup completed
  TSKM_BOOL_t isAvailable;    // Availability reception completed

  uint32_t watchCnt;          // Service monitoring counter
  uint32_t waitResCnt;        // Wait for response to request       DO_WAKEUP  - WAIT_WAKEUP
  uint32_t waitReqCnt;        // Wait for request issuance          REQ_WAKEUP - DO_WAKEUP
  TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];  //         REQ_WAKEUP - DO_WAKEUP

  uint32_t errTermCnt;        // Number of abnormal terminations    ALL
} TSKM_SVC_CTX_t;

// Service list
typedef struct {
  uint32_t svcNum;
  TSKM_SVC_CTX_t *svcList;
} TSKM_SVCS_CTX_t;

// State of waiting for service termination
typedef enum {
  TSKM_SVC_WAIT_NONE,       // No wait
  TSKM_SVC_WAIT_TRANSIENT,  // Waiting for termination of nonresident service
  TSKM_SVC_WAIT_BOTH,       // Waiting for termination of resident/nonresident service
} TSKM_SVC_WAIT_STATE_t;

// Service list
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsEventHandle
  /// \~english @par Summary
  ///
  /// \~english @param
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in] p_inEv
  ///            p_inEv   - pointer of input event info
  /// \~english @par
  ///           p_inEv   TSKM_EVENT_INFO_t*
  /// \~english @code
  /// typedef struct _TSKM_EVENT_INFO_t {
  ///   TSKM_EVENT_t event;
  ///   TSKM_ERR_t errCode;
  ///   pid_t fromPid;
  ///   TSKM_BOOL_t hasExtend;
  ///   uint32_t extendSize;
  ///   void *extendPrm;
  ///   union {
  ///     // TSKM->PRI
  ///     TSKM_EV_PRI_REQ_WAKEUP_PRM_t reqWakeup;
  ///     TSKM_EV_PRI_REQ_DOWN_PRM_t reqDown;
  ///     // PRI->TSKM
  ///     TSKM_EV_PRI_REP_WAKEUP_COMP_PRM_t repWakeupComp;
  ///     TSKM_EV_PRI_REP_DOWN_COMP_PRM_t repDownComp;
  ///     TSKM_EV_PRI_REP_CONNECT_PRM_t repConnect;
  ///     TSKM_EV_PRI_RES_WAKEUP_PRM_t resWakeup;
  ///     TSKM_EV_PRI_RES_DOWN_PRM_t resDown;
  ///     TSKM_EV_LCL_CHG_SVC_STATE_PRM_t chgSvc;
  ///   } prm;
  /// } TSKM_EVENT_INFO_t;
  ///           @endcode
  /// \~english @param  [out] p_outEv
  ///           p_outEv   - pointer of output event info
  /// \~english @par
  ///           p_outEv   TSKM_EVENT_INFO_t*
  /// \~english @retval
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcsEventHandle(TSKM_SVCS_CTX_t* p_svcs,
                                const TSKM_EVENT_INFO_t* p_inEv,
                                TSKM_EVENT_INFO_t* p_outEv);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsGetSvcBySvcId
  /// \~english @par Summary
  ///           find service in inputed service list by service id.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services object
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in] svcId
  ///            svcId  - service Id
  /// \~english @par
  ///            svcId   uint32_t
  /// \~english @retval TSKM_SVC_CTX_t*  service pointer
  /// \~english @retval TSKM_SVC_CTX_t*  NULL
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  If not find svcId in services list. [NULL]
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       -  method only
  /// \~english @par Detail
  ///           find service in inputed service list by service id.. If do not find then return NULL pointer.
  /// \~english @see getSvcCtxBySvcId
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_SVC_CTX_t* tskm_svcsGetSvcBySvcId(TSKM_SVCS_CTX_t* p_svcs,
                                       TSKM_SVCID_t svcId);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsGetSvcByPid
  /// \~english @par Summary
  ///            find service in inputed service list by process id.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in] pid
  ///            pid    - process id
  /// \~english @par
  ///            pid   pid_t
  /// \~english @retval TSKM_SVC_CTX_t*  service pointer
  /// \~english @retval TSKM_SVC_CTX_t*  NULL
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  If not find process id in services list. [NULL]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method only
  /// \~english @par Detail
  ///           find service in inputed service list by process id. If do not find then return NULL pointer.
  /// \~english @see  getSvcCtxByPid
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_SVC_CTX_t* tskm_svcsGetSvcByPid(TSKM_SVCS_CTX_t* p_svcs, pid_t pid);


  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsIsWaiting
  /// \~english @par Summary
  ///           Is the service state waiting.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_BOOL_t
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  If service state is TSKM_SVC_WAITCONNECT and wait response num is 0. [TSKM_FALSE]
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       -  method only
  /// \~english @par Detail
  ///          Is the service state waiting. \n
  ///          There are not service's state equal TSKM_SVC_WAITCONNECT or wait respose number > 0 return TSKM_TRUE.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_BOOL_t tskm_svcsIsWaiting(TSKM_SVCS_CTX_t* p_svcs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsGetSvcTermWaitState
  /// \~english @par Summary
  ///           Get the service which state equanl not equal terminal.
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_SVC_WAIT_STATE_t
  /// \~english @par
  ///            TSKM_SVC_WAIT_STATE_t  enum
  /// \~english @code
  /// typedef enum {
  ///   TSKM_SVC_WAIT_NONE,       // not waiting state
  ///   TSKM_SVC_WAIT_TRANSIENT,  // dynamic service terminal waiting state
  ///   TSKM_SVC_WAIT_BOTH,       // no dynamic/ dyanmic service terminal  waiting state
  /// } TSKM_SVC_WAIT_STATE_t;
  ///            @endcode
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///           If all services state equal TSKM_SVC_DORMANT. [TSKM_SVC_WAIT_NONE]
  ///           If dynamic service.  [TSKM_SVC_WAIT_TRANSIENT]
  ///           If not dynamic service and state not TSKM_SVC_FINDOWN. [TSKM_SVC_WAIT_BOTH]
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       -  method only
  /// \~english @par Detail
  ///           Get the service which state equanl not equal terminal. \n
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_SVC_WAIT_STATE_t tskm_svcsGetSvcTermWaitState(TSKM_SVCS_CTX_t* p_svcs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsSetBootInfo
  /// \~english @par Summary
  ///             initialize all services boot info.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in] p_info
  ///           p_info   - SS_SM_START 'FrameworkunifiedOnStart'/'evStart' message pointer
  /// \~english @par
  ///           p_info   T_SS_SM_START_DataStructType*
  /// \~english @code
  /// Data passed as part of SS_SM_START 'FrameworkunifiedOnStart'/'evStart' message
  /// typedef struct T_SS_SM_START_DataStruct {
  ///     EPWR_WAKEUP_FACTOR_TYPE   startupReason;
  ///     BOOL                      isUserModeOn;
  ///     ESMDataResetModeInfo      dataResetMode;
  ///     EPWR_SC_SECURITY_STATUS   securityStatus;
  ///     EPWR_SC_WAKEUP_TYPE       wakeupType;
  ///     ESMDramBackupStatus       dramBackupStatus;
  ///     ESMResetStatus            resetStatus;
  ///     UI_32                     errResetCount;
  ///     T_SS_SM_START_DataStruct
  /// } T_SS_SM_START_DataStructType;
  ///           @endcode
  /// \~english @param  [in] p_exInfo
  ///           p_exInfo   - FrameworkunifiedOnStart extend info pointer
  /// \~english @par
  ///           p_exInfo   T_SS_SM_START_ExtDataStructType*
  /// \~english @code
  /// /// Extended Parameter for FrameworkunifiedOnStart
  /// typedef struct {
  ///         BOOL             isProgUpdated;
  ///         EMRelaunchStatus relaunchStatus;
  ///         BOOL             isMapUpdated;
  ///         BOOL             isMapDiffUpdated;
  ///         uint8_t          reserved[SS_SM_START_EXT_RSV_SIZE];
  /// } T_SS_SM_START_ExtDataStructType;
  ///          @endcode
  /// \~english @retval TSKM_E_OK
  /// \~english @par
  ///            TSKM_SVC_WAIT_STATE_t  enum
  /// \~english @par Preconditions
  ///       -  call tskm_dataInitAll() to get boot info
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  none
  /// \~english @par Classification
  ///       -  pulbic
  /// \~english @par Type
  ///       -  method only
  /// \~english @par Detail
  ///      - initialize all services boot info.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcsSetBootInfo(TSKM_SVCS_CTX_t* p_svcs,
                                T_SS_SM_START_DataStructType* p_info,
                                T_SS_SM_START_ExtDataStructType *p_exInfo);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsAvtiveSvcTerm
  /// \~english @par Summary
  ///           Terminal the dynamic services.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  tskm_svcDownRequest return failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       -  pulbic
  /// \~english @par Type
  ///       - method only
  /// \~english @par Detail
  ///       Terminal all the dynamic services which state equal TSKM_SVC_RUNNING and response num equal 0. \n
  /// \~english @see  tskm_svcDownRequest
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcsAvtiveSvcTerm(TSKM_SVCS_CTX_t* p_svcs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsCallDebugDump
  /// \~english @par Summary
  ///           Send DebugDump message to services.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  tskm_sockSend send DebugDump message failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method only
  /// \~english @par Detail
  ///            Send DebugDump message to services which state equal TSKM_SVC_RUNNING. \n
  /// \~english @see  tskm_sockSend
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcsCallDebugDump(TSKM_SVCS_CTX_t* p_svcs);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcsCallLowMem
  /// \~english @par Summary
  ///           Send checking low memory message to services.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       - tskm_sockSend send LowMemory message failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method only
  /// \~english @par Detail
  ///       Send checking LowMemory message to services which state equal TSKM_SVC_RUNNING. \n
  /// \~english @see  tskm_sockSend
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcsCallLowMem(TSKM_SVCS_CTX_t* p_svcs);

// Service
  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcEventHandle
  /// \~english @par Summary
  ///           process event.
  /// \~english @param  [in] p_svcs
  ///            p_svcs   - pointer of services
  /// \~english @par
  ///           p_svcs   TSKM_SVCS_CTX_t*
  /// \~english @code
  ///           typedef struct {
  ///             uint32_t svcNum;           // service num
  ///             TSKM_SVC_CTX_t *svcList;   // services list pointer
  ///           } TSKM_SVCS_CTX_t;
  ///           @endcode
  /// \~english @param  [in][out] p_ev
  ///            p_ev   - event info pointer
  /// \~english @par
  ///           p_ev   TSKM_EVENT_INFO_t*
  /// \~english @code
  /// typedef struct _TSKM_EVENT_INFO_t {
  ///   TSKM_EVENT_t event;
  ///   TSKM_ERR_t errCode;
  ///   pid_t fromPid;
  ///   TSKM_BOOL_t hasExtend;
  ///   uint32_t extendSize;
  ///   void *extendPrm;
  ///   union {
  ///     // TSKM->PRI
  ///     TSKM_EV_PRI_REQ_WAKEUP_PRM_t reqWakeup;
  ///     TSKM_EV_PRI_REQ_DOWN_PRM_t reqDown;
  ///
  ///     // PRI->TSKM
  ///     TSKM_EV_PRI_REP_WAKEUP_COMP_PRM_t repWakeupComp;
  ///     TSKM_EV_PRI_REP_DOWN_COMP_PRM_t repDownComp;
  ///     TSKM_EV_PRI_REP_CONNECT_PRM_t repConnect;
  ///     TSKM_EV_PRI_RES_WAKEUP_PRM_t resWakeup;
  ///     TSKM_EV_PRI_RES_DOWN_PRM_t resDown;
  ///     TSKM_EV_LCL_CHG_SVC_STATE_PRM_t chgSvc;
  ///   } prm;
  /// } TSKM_EVENT_INFO_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  none
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///         process event.\n
  ///         process connect response.\n
  ///         process disconnect response. \n
  ///         process service wakeup request. \n
  ///         process service down request. \n
  ///         process service debugdump request. \n
  ///         process service exit request. \n
  ///         process service terminal response. \n
  /// \~english @see  isSvcEvent,connectHandle,disConnectHandle,resWakeupHandle, \n
  ///                  resDownHandle,resDebugDumpHandle,reqExit,repTermHandle \n
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcEventHandle(TSKM_SVC_CTX_t* p_svc, TSKM_EVENT_INFO_t* p_ev);  // p_ev: [IN/OUT]

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcExec
  /// \~english @par Summary
  ///           start service.
  /// \~english @param  [in][out] p_svc
  ///            p_svc   - pointer of services
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  ///            TSKM_E_STATE
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///            - input parameter p_svc is NULL. [TSKM_E_NG]
  ///            - service state is TSKM_SVC_DISABLE [TSKM_E_STATE]
  ///            - reqTouch return failed [TSKM_E_NG]
  ///            - tskm_pf_createProc return failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///       start service which state is TSKM_SVC_DORMANT.\n
  /// \~english @see  svcExec, reqTouch, tskm_pf_createProc
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcExec(TSKM_SVC_CTX_t* p_svc);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcWakeupRequest
  /// \~english @par Summary
  ///           Send started request to service.
  /// \~english @param  [in][out] p_svc
  ///            p_svc   - pointer of service
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @param  [in] p_req
  ///            p_req   - pointer of request
  /// \~english @par
  ///           p_req   TSKM_GSTEP_REQ_INFO_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVCID_t svcId;           // service ID
  ///   TSKM_LOCAL_STEP_t localStep;  // local step id
  /// } TSKM_GSTEP_REQ_INFO_t;
  ///            @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  ///            TSKM_E_STATE
  /// \~english @par Preconditions
  ///       -  none
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///            - Input parameter p_svc is NULL. [TSKM_E_STATE]
  ///            - Service state is TSKM_SVC_DISABLE [TSKM_E_STATE]
  ///            - wakeupRequest return failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///       Send started request to service. If service do not connect, save the request.\n
  ///       Send request again when service connect. \n
  /// \~english @see  svcWakeupRequest
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcWakeupRequest(TSKM_SVC_CTX_t* p_svc,
                                 TSKM_GSTEP_REQ_INFO_t* p_req);


  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcDownRequest
  /// \~english @par Summary
  ///           Send service down request to service.
  /// \~english @param  [in][out] p_svc
  ///            p_svc   - pointer service
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @param  [in] p_req
  ///            p_req   - request pointer
  /// \~english @par
  ///           p_req   TSKM_GSTEP_REQ_INFO_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVCID_t svcId;           // service ID
  ///   TSKM_LOCAL_STEP_t localStep;  // local step id
  /// } TSKM_GSTEP_REQ_INFO_t;
  ///            @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_NG
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  Send service down request failed [TSKM_E_NG]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///       If service state is commounicated(TSKM_SVC_WAKEUP, TSKM_SVC_RUNNING), \n
  ///            send service down request and set state as TSKM_SVC_DOWN.\n
  /// \~english @see  tskm_svcIsCommunicatable,tskm_sockSend
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcDownRequest(TSKM_SVC_CTX_t* p_svc,
                               TSKM_GSTEP_REQ_INFO_t* p_req);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcDisableRequest
  /// \~english @par Summary
  ///
  /// \~english @param  [in][out] p_svc
  ///            p_svc   - pointer of service
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  ///            TSKM_E_STATE
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  If service state is not TSKM_SVC_DORMANT and TSKM_SVC_DISABLE. [TSKM_E_STATE]
  /// \~english @par Classification
  ///       -  public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///       Set service state as TSKM_SVC_DISABLE if state is TSKM_SVC_DORMANT.
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcDisableRequest(TSKM_SVC_CTX_t* p_svc);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcDisableRequest
  /// \~english @par Summary
  ///           Set service state as start available.
  /// \~english @param  [in][out] p_svc
  ///            p_svc   - Pointer to service
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_ERR_t
  /// \~english @par
  ///            TSKM_E_OK
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///       -  none
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///           Set service state as start available which state is disable(TSKM_SVC_DISABLE).
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_ERR_t tskm_svcEnableRequest(TSKM_SVC_CTX_t* p_svc);

  /////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup tskm_svcIsCommunicatable
  /// \~english @par Summary
  ///           Whether service is Communicatable.
  /// \~english @param  [in] p_svc
  ///            p_svc   - pointer of service
  /// \~english @par
  ///           p_svc   TSKM_SVC_CTX_t*
  /// \~english @code
  /// typedef struct {
  ///   TSKM_SVC_ATTR_t *attr;      // attribute of service
  ///   TSKM_SVC_STATE_t state;     // state of service
  ///   int iFd;                    // inotifyFd used by touch
  ///   pid_t pid;                  // PID of service
  ///   int connFd;                 // service communicate fd
  ///   T_SS_SM_START_DataStructType bootInfo;        // BOOT info
  ///   T_SS_SM_START_ExtDataStructType extBootInfo;  // extend BOOT info
  ///   TSKM_BOOL_t isShmDone;      // shared memory init done
  ///   TSKM_BOOL_t isStepDone;     // step done
  ///   TSKM_BOOL_t isAvailable;    // Availability flag
  ///   uint32_t watchCnt;          // service monitor count
  ///   uint32_t waitResCnt;        // wait response num
  ///   uint32_t waitReqCnt;        // wait request num
  ///   TSKM_GSTEP_REQ_INFO_t request[TSKM_SVC_WAIT_REQ_MAX];
  ///   uint32_t errTermCnt;        // exception terminal count
  /// } TSKM_SVC_CTX_t;
  ///           @endcode
  /// \~english @retval TSKM_BOOL_t
  /// \~english @par
  ///            TSKM_FALSE
  ///            TSKM_TRUE
  /// \~english @par Preconditions
  ///       -
  /// \~english @par Change of the internal state
  ///       -  The internal state is not changed.
  /// \~english @par Causes of failures
  ///            - If input parameter p_svc is NULL.  [TSKM_FALSE]
  ///            - If input parameter p_svc->attr->type is TSKM_SVC_TYPE_UNKNONW. [TSKM_FALSE]
  ///            - If service state is TSKM_SVC_DORMANT or TSKM_SVC_WAITCONNECT or
  ///              TSKM_SVC_FINDOWN orTSKM_SVC_DISABLE. [TSKM_FALSE]
  /// \~english @par Classification
  ///       - public
  /// \~english @par Type
  ///       - method
  /// \~english @par Detail
  ///       Whether service is Communicatable. \n
  ///       If service state is TSKM_SVC_WAKEUP or TSKM_SVC_RUNNING or TSKM_SVC_DOWN, return TSKM_TRUE. \n
  /// \~english @see  None
  ////////////////////////////////////////////////////////////////////////////////////
TSKM_BOOL_t tskm_svcIsCommunicatable(TSKM_SVC_CTX_t* p_svc);

#endif  // TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_SVC_H_
/** @}*/
/** @}*/
/** @}*/
