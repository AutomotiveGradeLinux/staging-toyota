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

#include <unistd.h>
#include <signal.h>

#include <native_service/frameworkunified_framework_if.h>

#include "tskm_debug.h"
#include "tskm_api.h"

TSKM_ERR_t TSKM_SvcCtl(TSKM_SVCID_t svcId, const TSKM_SVC_CTL_t* ctl) {
  TSKM_ERR_t tskmRet;
  int ii;

  for (ii = 0; ii < 3; ii++) {
    // Retry three times
    tskmRet = _TSKM_SvcCtl(svcId, ctl);
    if (tskmRet == TSKM_E_RETRY) {
      usleep(300 * 1000);  // 300ms wait
    } else {
      break;
    }
  }

  return tskmRet;
}

TSKM_ERR_t TSKM_ErrorReboot(const TSKM_ERROR_REBOOT_t* p_info) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;
  RPC_Result rpcRet;

  rpcRet = _TSKM_ErrorReboot(p_info);
  if (RPC_OK != rpcRet) {
    tskmRet = TSKM_E_PAR;
  }

  return tskmRet;
}

TSKM_ERR_t TSKM_DataInit(HANDLE hApp, const TSKM_DATAINIT_t *p_info) {
  TSKM_ERR_t tskmRet = TSKM_E_NG;
  TSKM_PRINTF(TSKM_LOG_API, "%s()", __FUNCTION__)

  if (!p_info || !hApp || (p_info->type != TSKM_DATAINIT_TYPE_USER)
      || !(p_info->onCompInit)) {
    TSKM_ASSERT(0);
    return TSKM_E_PAR;
  }

  // LCOV_EXCL_BR_START 10: Due to compiler dependency
  EFrameworkunifiedStatus l_eStatus = FrameworkunifiedAttachCallbackToDispatcher(hApp,
  SS_TASK_MANAGER,
                                                       TSKM_DATAINIT_RESP,
                                                       p_info->onCompInit);
  // LCOV_EXCL_BR_STOP
  if (eFrameworkunifiedStatusOK != l_eStatus) {
    TSKM_ASSERT(0);
  } else {
    HANDLE hSvc = FrameworkunifiedOpenService(hApp, SS_TASK_MANAGER);
    if (hSvc == NULL) {
      TSKM_ASSERT(0);
      tskmRet = TSKM_E_STATE;
    } else {
      if (eFrameworkunifiedStatusOK != FrameworkunifiedSendMsg(hSvc, TSKM_DATAINIT_REQ, 0, NULL)) {
        TSKM_ASSERT(0);
      } else {
        tskmRet = TSKM_E_OK;
      }
    }

    TSKM_ASSERT(eFrameworkunifiedStatusOK == FrameworkunifiedCloseService(hApp, hSvc));
  }

  return tskmRet;
}  // LCOV_EXCL_BR_LINE 10: Final line

