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
 * @file tskm_local_type.h
 * @brief \~english localtype of tskm
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
#ifndef TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_LOCAL_TYPE_H_
#define TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_LOCAL_TYPE_H_


#include <stdint.h>
#include <sys/types.h>
#include <native_service/frameworkunified_types.h>
#include "system_service/INI_API.hpp"
#include "system_service/tskm_type.h"

//==================================================
//  Configuration
//==================================================

#if 1
#define TSKM_CFG_WAIT_SHUTDOWN 10000
#else
#define TSKM_CFG_WAIT_SHUTDOWN 1      // Handy for test
#warning TSKM_CFG_WAIT_SHUTDOWN is temorary
#endif

#define TSKM_CFG_TOUCH_TIMEOUT     2  // TOUCH time-out

//==================================================
//  Task manager internal
//==================================================
// State
#define TSKM_ST_ACCOFF      0x01000000U   // ACC-OFF (Default) No transition here
#define TSKM_ST_ACCON       0x02000000U   // ACC-ON
#define TSKM_ST_WAKEUP      0x02010000U   // Gradual startup
#define TSKM_ST_RUNNING     0x02020000U   // Running (Startup completed)
#define TSKM_ST_DOWN        0x02040000U   // Gradual Terminating

typedef uint32_t TSKM_STATE_t;
#define TSKM_STATE_MASK0    0xFF000000U
#define TSKM_STATE_MASK1    0xFFFF0000U
#define TSKM_STATE_MASK2    0xFFFFFF00U
#define TSKM_STATE_MASK3    0xFFFFFFFFU

// Global step IDs for gradual starting
typedef uint32_t TSKM_GLOBAL_STEP_t;
#define TSKM_GSTEP_NONE        0x00000000U  //
#define TSKM_GSTEP_BUPCHK      0x00000001U

// Local step
typedef uint32_t TSKM_LOCAL_STEP_t;
#define TSKM_LSTEP_LAST   0xFFFFFFF0U   // Executing to end of local steps
#define TSKM_LSTEP_SHM    0xFFFFFFF1U   // Creating shared memory
#define TSKM_LSTEP_BUPCHK 0xFFFFFFF2U   // Checking backup memory
#define TSKM_LSTEP_ALL    0xFFFFFFF3U   // Executing all user definitions

/************************************************************
 *  Structure of gradual startup/termination structure
    (When this structure is modified, structure of parsexml is also changed)
 *************************************************************/
// Structure of request for services
typedef struct {
  TSKM_SVCID_t svcId;           // Service ID
  TSKM_LOCAL_STEP_t localStep;  // Local step number
} TSKM_GSTEP_REQ_INFO_t;

// Structure of gradual startup/termination
typedef struct {
  uint32_t gstepId;

  // List of executing services
  uint32_t execSvcNum;
  TSKM_SVCID_t* execSvcIdList;

  // List of request
  uint32_t reqNum;
  TSKM_GSTEP_REQ_INFO_t* reqList;

  // Event completion wait conditions
  uint64_t nextTransCond;
} TSKM_GSTEP_t;

typedef struct {
  uint32_t gstepIdx;            // Step index
  uint32_t gstepNum;            // Step number
  TSKM_GSTEP_t* gstep;          // Gradual startup/termination info
  uint64_t compState;           // Status of receiving for Event completion notification
} TSKM_GSTEP_CTX_t;

//********* Non-volatile management information ****************

#define TSKM_NV_STRUCT_VERSION  "v000"  // Compatible version of non-volatile management structure 
#define TSKM_SVC_RESERVE_MAX    (8)     // Maximum number of reserved services to startup

#define TSKM_NV_SIZE_ALL     (512)
#define TSKM_NV_SIZE_HEADER   (32)
#define TSKM_NV_SIZE_FOOTER   (32)
#define TSKM_NV_SIZE_BODY   (TSKM_NV_SIZE_ALL - TSKM_NV_SIZE_HEADER - TSKM_NV_SIZE_FOOTER)

// Header
typedef struct {
  char version[5];
} TSKM_NV_HEADER_t;

// Footer
typedef struct {
  uint32_t checkSum;
} TSKM_NV_FOOTER_t;

// Body
typedef struct {
  uint8_t rsvSvcNum;                            // Number of reserved services
  TSKM_SVCID_t rsvSvcs[TSKM_SVC_RESERVE_MAX];   // List of services
} TSKM_NV_BODY_t;

// Non-volatile storage info
typedef struct {
  TSKM_NV_HEADER_t header;
  uint8_t rsvHeader[TSKM_NV_SIZE_HEADER - sizeof(TSKM_NV_HEADER_t)];
  TSKM_NV_BODY_t body;
  uint8_t rsvBody[TSKM_NV_SIZE_BODY - sizeof(TSKM_NV_BODY_t)];
  uint8_t rsvFooter[TSKM_NV_SIZE_FOOTER - sizeof(TSKM_NV_FOOTER_t)];
  TSKM_NV_FOOTER_t footer;
} TSKM_NV_INFO_t;

// Internal event
typedef enum {
  TSKM_EV_NOP,                  // Do nothing
  // TSKM->PRI
  TSKM_EV_PRI_REQ_WAKEUP,       // Startup request
  TSKM_EV_PRI_REQ_DOWN,         // Termination request
  TSKM_EV_PRI_REQ_TOUCH,
  TSKM_EV_PRI_REQ_DEBUGDUMP,    // DebugDump request
  TSKM_EV_PRI_REP_LOWMEM,       // LowMemory detection request

  // PRI->TSKM(SVC)
  TSKM_EV_PRI_REP_WAKEUP_COMP,  // Notification of event completion at startup
  TSKM_EV_PRI_REP_DOWN_COMP,    // Notification of event completion at termination
  TSKM_EV_PRI_REP_CONNECT,      // Connecting
  TSKM_EV_PRI_REP_DISCONNECT,   // Disconnecting
  TSKM_EV_PRI_RES_WAKEUP,       // Response to startup request
  TSKM_EV_PRI_RES_DOWN,         // Response to termination request
  TSKM_EV_PRI_RES_DEBUGDUMP,    // Response to DebugDump request
  TSKM_EV_PRI_REQ_EXIT,         // Response to terminating request

  // SVC->TSKM
  TSKM_EV_SVC_REP_TERM,         // Termination of service

  // APL->TSKM
  TSKM_EV_API_REQ_REBOOT,       // Reboot system
  TSKM_EV_API_REQ_SVC_CTL,      // Startup service
  TSKM_EV_API_REQ_SVC_DISABLE,  // Prohibit starting service

  // Internal message
  TSKM_EV_LCL_REQ_STOP,         // Termination request
  TSKM_EV_LCL_CHG_SVC_STATE,    // State Change of service
  TSKM_EV_LCL_REQ_SDUMP,        // Dump of service list
  TSKM_EV_LCL_REP_TIMEOUT,      // Occured Timeout
  TSKM_EV_LCL_REP_POLLING,      // Polling request
  TSKM_EV_LCL_REP_LOWMEM,       // Notification of LowMemory detection
  TSKM_EV_LCL_REQ_TRANS_STEP,   // Gradual transitioning request
} TSKM_EVENT_t;

#define TSKM_EV_BOOTINFO_SIZE    sizeof(T_SS_SM_START_DataStructType)
#define TSKM_EV_EXTBOOTINFO_SIZE sizeof(T_SS_SM_START_ExtDataStructType)

typedef struct {
  TSKM_SVCID_t svcId;
  TSKM_LOCAL_STEP_t localStep;
  char bootInfo[TSKM_EV_BOOTINFO_SIZE];
  char extBootInfo[TSKM_EV_EXTBOOTINFO_SIZE];
  TSKM_BOOL_t isDynamic;  // Nonresident task?
} TSKM_EV_PRI_REQ_WAKEUP_PRM_t;

typedef struct {
  TSKM_LOCAL_STEP_t localStep;
} TSKM_EV_PRI_REQ_DOWN_PRM_t;

// PRI->TSKM
typedef struct {
  uint64_t compId;
} TSKM_EV_PRI_REP_WAKEUP_COMP_PRM_t;

typedef struct {
  uint64_t compId;
} TSKM_EV_PRI_REP_DOWN_COMP_PRM_t;

typedef struct {
  int connFd;
} TSKM_EV_PRI_REP_CONNECT_PRM_t;

typedef struct {
  TSKM_BOOL_t isLast;
  TSKM_BOOL_t isShmDone;
  TSKM_BOOL_t isStepDone;
} TSKM_EV_PRI_RES_WAKEUP_PRM_t;

typedef struct {
  TSKM_BOOL_t isLast;
} TSKM_EV_PRI_RES_DOWN_PRM_t;

#define TSKM_EV_DEBUGDUMP_SIZE 4096

typedef struct {
  char dumpMsg[TSKM_EV_DEBUGDUMP_SIZE];
} TSKM_EV_PRI_EX_RES_DEBUGDUMP_PRM_t;

// APL->TSKM

// Internal message
typedef struct {
  TSKM_SVCID_t svcId;
} TSKM_EV_LCL_CHG_SVC_STATE_PRM_t;

// The structure must be on a stack
typedef struct _TSKM_EVENT_INFO_t {
  TSKM_EVENT_t event;
  TSKM_ERR_t errCode;
  pid_t fromPid;
  TSKM_BOOL_t hasExtend;
  uint32_t extendSize;
  void *extendPrm;
  union {
    // TSKM->PRI
    TSKM_EV_PRI_REQ_WAKEUP_PRM_t reqWakeup;
    TSKM_EV_PRI_REQ_DOWN_PRM_t reqDown;

    // PRI->TSKM
    TSKM_EV_PRI_REP_WAKEUP_COMP_PRM_t repWakeupComp;
    TSKM_EV_PRI_REP_DOWN_COMP_PRM_t repDownComp;
    TSKM_EV_PRI_REP_CONNECT_PRM_t repConnect;
    TSKM_EV_PRI_RES_WAKEUP_PRM_t resWakeup;
    TSKM_EV_PRI_RES_DOWN_PRM_t resDown;

    // Internal message
    TSKM_EV_LCL_CHG_SVC_STATE_PRM_t chgSvc;
  } prm;
  _TSKM_EVENT_INFO_t() {
    event = TSKM_EV_NOP;
    errCode = TSKM_E_OK;
    fromPid = 0;
    hasExtend = TSKM_FALSE;
    extendSize = 0;
    extendPrm = NULL;
  }
  ~_TSKM_EVENT_INFO_t() {
    if (hasExtend && !extendPrm) {
      free(extendPrm);
    }
  }
} TSKM_EVENT_INFO_t;

typedef enum {
  TSKM_DATAINIT_REQ = 800,
  TSKM_DATAINIT_RESP,
  TSKM_TRANS_STEP_REQ
} TSKM_PROTCOL_t;

#endif  // TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_TSKM_LOCAL_TYPE_H_
/** @}*/
/** @}*/
/** @}*/
