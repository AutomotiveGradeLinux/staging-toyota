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
 * @file Primary_common.h
 * @brief \~english This file contains declaration of common enum and structures
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
#ifndef TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_PRIMARY_COMMON_H_
#define TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_PRIMARY_COMMON_H_

#include <stdint.h>
#include <pthread.h>

typedef unsigned int PRI_DWORD;    // To eliminate the DWORD

/* SRAM process type */
#define SRAM_INIT   (int)0        /* Initialization processing specification */
#define SRAM_CHK    (int)1        /* Range check processing specification */
#define SRAM_MAPVER (int)2        /* Map Version change support designation */
#define SRAM_MAPVER_CHK (int)3    /* Map Version change support + range checking specification */
#define SRAM_SYSTEM_INIT (int)4   /* System initialization processing specification */
#define SRAM_CONFIG_INIT (int)5   /* Reset setting */

/* Processing result */
#define SRAM_CHK_OK   (int)0      /* SRAM checking completed normally (no change) */
#define SRAM_CHK_CHG  (int)1      /* SRAM checking Successful completion value is changed (Including during initialization) */
#define SRAM_CHK_NG   (int)-1     /* SRAM checking error */

/*
 Constant definition
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define PRIM_SHM_NAME_MAX           32              /* Maximum name size */

// Local stage number of the stage start request/end request from the TSKM
// In XML, it is set with a direct value of 1-9, but it is left because the legacy service refers to this definition
enum _PRIM_FORK_STEP {
  PRIM_STEPFORK_FIRST = 1, /* First startup step */
  PRIM_STEPFORK_SECOND,    /* 2nd startup step */
  PRIM_STEPFORK_THIRD,     /* 3rd startup step */
  PRIM_STEPFORK_FOURTH,    /* 4th startup step */
  PRIM_STEPFORK_FIFTH,     /* 5th startup step */
  PRIM_STEPFORK_SIXTH,     /* 6th startup step */
  PRIM_STEPFORK_SEVENTH,   /* 7th startup step */
  PRIM_STEPFORK_EIGHTH,    /* 8th startup step */ /* #GPF_10_018 */
  PRIM_STEPFORK_NINTH,     /* 9th startup step */ /* #GPF_10_018 */
  PRIM_STEPFORK_TENTH,     /* 10th startup step */ /* #GPF_10_018 */
  PRIM_STEPFORK_MAX        /* Maximum number of Gradual startup steps */ /* Used for judgment when receiving gradual startup */
};

enum _PRIM_ACCOFF_STEP {
  PRIM_ACCOFF_FIRST = 1, /* First step */
  PRIM_ACCOFF_SECOND,    /* 2nd step */
  PRIM_ACCOFF_THIRD,     /* 3rd step */
  PRIM_ACCOFF_FOURTH,    /* 4th step */
  PRIM_ACCOFF_FIFTH,     /* 5th step */
  PRIM_ACCOFF_MAX        /* Total number of ACC-OFF steps */ /* Used for judgment when receiving ACC-OFF instruction */
};

/******************************************************************************
 * TAG        : PRIM_SHAREDATA_TBL
 * ABSTRACT   : Shared data table
 * NOTE       :
 ******************************************************************************/
/* Structure of shared data table */
typedef struct {
  char shmName[PRIM_SHM_NAME_MAX];  /* Shared data name */
  int32_t size;                     /* Shared data size */
} PRIM_SHAREDATA_TBL;

/******************************************************************************
 * TAG        : PRIM_FORK_TBL
 * ABSTRACT   : Startup process table
 * NOTE       :
 ******************************************************************************/
typedef struct {
  PRI_DWORD (*threadFunc)(void* prm); /* Thread function */
  void* prm; /* Thread startup parameters */
  uint32_t priority; /* Thread priority */
  uint16_t pno; /* Thuread number */
  uint8_t rsv; /* Reserved field */
  uint8_t wakeLocalStep; /* Startup step */
  // The following is valid only when useLegacy = FALSE
  char threadName[16]; /* Thread name */
  char downLocalStep; /* When the termination step is 0, termination notification is not issued */
  void* primaryLocal; /* Local area for libPrimary (cannot be changed) */
} PRIM_FORK_TBL;

/******************************************************************************
 * TAG        : PRIM_EXFUNC_TBL
 * ABSTRACT   : Feature extension table
 ******************************************************************************/
typedef struct {
  void (*func)(void* prm); /* Feature extension function */
  void* prm; /* parameters         */
  uint8_t localStep; /* Startup step */
  uint8_t rsv[3]; /* Reserved     */
} PRIM_EXFUNC_TBL;

/******************************************************************************
 * TAG        : PRIM_BACKUPCHK_TBL
 * ABSTRACT   : Function table for check backup data
 ******************************************************************************/
typedef struct {
  int32_t (*func)(int32_t kind); /* Feature Extension Functions */
} PRIM_BACKUPCHK_TBL;

/******************************************************************************
 * TAG        : PRIM_ACCOFF_TBL
 * ABSTRACT   : ACC-OFF process table
 ******************************************************************************/
typedef struct {
  uint32_t localStep; /* ACC-OFF process step  */
  uint16_t pno; /* PNO of Notification destination        */
  uint8_t rsv[2];
} PRIM_ACCOFF_TBL;

#endif  // TASK_MANAGER_SERVER_INCLUDE_SYSTEM_SERVICE_PRIMARY_COMMON_H_
/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 End of File : Primary_common.h
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */
/** @}*/
/** @}*/
/** @}*/
