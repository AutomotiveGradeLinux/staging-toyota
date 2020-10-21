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
* @file LineSensDrv_Thread.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files

#include "LineSensDrv_Thread.h"
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include "positioning_def.h"
#include "positioning_common.h"
/*---------------------------------------------------------------------------*/
// Global variable

static u_int8   g_sys_recv_flg = 0;         // For debugging
char g_threadname[] = "POS_Sens";
uint8_t g_uc_vehicle_reverse = 0;
uint8_t g_uc_reverse_state = 0;
static BOOL g_line_sens_thread_stop = FALSE;

#define POS_REV_STATE_PROCESSING 0
#define POS_REV_STATE_IDLE 1

/*******************************************************************************
 * MODULE   : LineSensDrv_Thread
 * ABSTRACT : LineSensor driver thread main process
 * FUNCTION : Main processing
 * ARGUMENT : lpvPara  :
 * NOTE     :
 * RETURN   :
 ******************************************************************************/
EFrameworkunifiedStatus LineSensDrvThread(HANDLE h_app) {
  int32           l_ret      = RET_LSDRV_SUCCESS;
  EFrameworkunifiedStatus l_status = eFrameworkunifiedStatusOK;


  (void)PosSetupThread(h_app, ETID_POS_SENS);

  /* Execute the initialization processes */
  l_ret = LineSensDrvMainThreadInit(h_app);
  if (RET_LSDRV_SUCCESS != l_ret) {
    l_status = eFrameworkunifiedStatusFail;
  }

  return l_status;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_MainThread_Init
 * ABSTRACT  : Thread initialization process
 * FUNCTION  : Initialize thread
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : RET_LSDRV_SUCCESS:Success in initialization
 *         RET_LSDRV_ERROR  :Initialization failed
 ******************************************************************************/
int32 LineSensDrvMainThreadInit(HANDLE h_app) {
  int32   l_ret  = RET_LSDRV_SUCCESS;
  BOOL    b_ret  = TRUE;

  /****** Global variable initialization **********/
  LineSensDrvParamInit();

  /****** LineSensDrvApi initialization **********/
  b_ret = LineSensDrvApiInitialize();

  if (TRUE != b_ret) {
    l_ret = RET_LSDRV_ERROR;
  }

  return l_ret;
}

/*******************************************************************************
 * MODULE    : LineSensDrv_Param_Init
 * ABSTRACT  : Global variable initialization processing
 * FUNCTION  : Initialize global variables
 * ARGUMENT  : None
 * NOTE      :
 * RETURN    : None
 ******************************************************************************/
void LineSensDrvParamInit(void) {
  LineSensDrvSpeedKmphDataInit();   // Data table initialization process for vehicle speed calculation
}

/**
 * @brief
 *   Pos_Sens thread stop processing
 */
void LineSensDrvThreadStopProcess(void) {
  g_line_sens_thread_stop = TRUE;

  if (POS_REV_STATE_IDLE == g_uc_reverse_state) {
    PosTeardownThread(ETID_POS_SENS);
  }

  return;
}

/**
 * @brief
 *   Get initial sensor data reception flag
 */
u_int8 LineSensDrvGetSysRecvFlag(void) {
  return g_sys_recv_flg;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
