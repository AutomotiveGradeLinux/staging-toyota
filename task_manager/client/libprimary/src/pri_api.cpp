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

#include "system_service/tskm_local_type.h"
#include "system_service/INI_API.hpp"
#include "tskm_debug.h"
#include "pri_main.h"

BOOL __thread isMain = FALSE;  // Check for accessibility from the main thread context

TSKM_STATIC BOOL isInitPrmValid(const T_PRIM_PRM* p_prm) {
  if (p_prm == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->shmTbl == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->wakeupExFuncTbl == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->downExFuncTbl == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->onInit == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->onDestory == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->onDebugDump == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  } else if (p_prm->onTouch == NULL) {
    TSKM_ASSERT(0);
    return FALSE;
  }

  return TRUE;
}

/******************************************************************************
 *      APIs for implementing the main thread on the service side
 *******************************************************************************/
/*********************************************************
 *  Primary library initialization
 *********************************************************/
int32_t INI_Init(T_PRIM_PRM* p_prm, int argc, char* argv[], int *fdNum,
                 int fdlist[INI_FD_MAX]) {
  isMain = TRUE;
  if (isInitPrmValid(p_prm) == FALSE) {
    TSKM_ASSERT(0);
    return INI_FALSE;
  }
  pri_init(p_prm, argc, argv, fdNum, fdlist);
  return INI_SUCCESS;
}

/***********************************************************
 *  Primary library / Event handler
 *  ret:TRUE  Running
 *  ret:FALSE Finished (Terminated a process with INI_Term())
 ************************************************************/
BOOL INI_Handler(fd_set* p_fds) {
  if (!isMain || NULL == p_fds) {
    TSKM_ASSERT(0);
    return FALSE;
  }

  return pri_handler(p_fds);
}

/***********************************************************
 *  Primary (Processing finished)
 ************************************************************/
void INI_Term(void) {
  pri_term();
}

/***********************************************************
 *  Primary (State setting for watching Services)
 ************************************************************/
int32_t INI_SetMonitorState(T_PRIM_MONITOR_PRM *p_prm) {
  if (!p_prm) {
    TSKM_ASSERT(0);
    goto ERROR;
  }

  return pri_setMonitorState(p_prm->bIsRun, p_prm->timeout);
  ERROR: return INI_FALSE;
}

/******************************************************************************
 *      Hiding the Main Thread in the Primary Library
 *******************************************************************************/
/************************************
 *  Startup FW MainLoop
 ************************************/
int INI_Main(T_PRIM_PRM* p_prm, int argc, char* argv[]) {
  if (isInitPrmValid(p_prm) == FALSE) {  // LCOV_EXCL_BR_LINE 6: Checked by Init()
    TSKM_ASSERT(0);
    return INI_FALSE;
  }

  isMain = TRUE;

  return pri_main(p_prm, argc, argv);
}

/******************************************************************************
 *      Common API
 *******************************************************************************/
/************************************
 *  Process termination
 ************************************/
void INI_ExitStart(void * rsv) {
  static int isCalled = 0;
  if (isCalled) {
    return;
  }
  isCalled = 1;
  pri_exitStart(rsv);
}

void INI_ExitDone(int status) {
  static int isCalled = 0;
  if (isCalled) {
    return;
  }
  isCalled = 1;
  pri_exitDone(status);
}

/************************************
 *  Private Info Acquisition
 ************************************/
void* INI_GetPrivate() {
  return pri_getPrivate();
}

/************************************
 *  Applicastion Handle Aquisition
 ************************************/
HANDLE INI_GetHandle() {
  return pri_getHandle();
}

/************************************
 *  Timeout setting for service monitoring status setting
 ************************************/
int32_t INI_SetMonitorTimeout(uint32_t timeout) {
  return pri_setMonitorTimeout(timeout);
}

/************************************
 *  Event completion notification at startup
 ************************************/
int32_t INI_StepForkComp(uint64_t compId) {
  return pri_stepForkComp(compId);
}

/************************************
 *  Event completion notification at termination
 ************************************/
int32_t INI_AccOffComp(uint64_t compId) {
  return pri_accOffComp(compId);
}

/************************************
 *  BOOT Info Acquisition
 ************************************/
int32_t INI_GetBootInfo(T_SS_SM_START_DataStructType *info) {
  if (info == NULL) {
    TSKM_ASSERT(0);
    return INI_FALSE;
  }
  return pri_getBootInfo(info);
}

/************************************
 *  Extended BOOT Info Acquisition
 ************************************/
int32_t INI_GetExtBootInfo(T_SS_SM_START_ExtDataStructType *info) {
  if (info == NULL) {
    TSKM_ASSERT(0);
    return INI_FALSE;
  }
  return pri_getExtBootInfo(info);
}

/************************************
 *  DebugDump Responding
 ************************************/
void _INI_DEBUGDUMP(BOOL bIsNeedSvcName, PCSTR cFormat, ...) {  // LCOV_EXCL_START 7: for debugging
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  va_list argList;
  char buf[TSKM_EV_DEBUGDUMP_SIZE] = { 0 };

  if (bIsNeedSvcName) {
    snprintf(buf, TSKM_EV_DEBUGDUMP_SIZE, "%s/",
             FrameworkunifiedGetAppName(pri_getHandle()));
  }

  va_start(argList, cFormat);
  vsnprintf(&buf[strlen(buf)], TSKM_EV_DEBUGDUMP_SIZE - strlen(buf), cFormat,
            argList);
  va_end(argList);

  return pri_sendDebugDumpRes(buf);
}
// LCOV_EXCL_STOP 7

