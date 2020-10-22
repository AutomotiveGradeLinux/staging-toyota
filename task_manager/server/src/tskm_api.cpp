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

#include "tskm_debug.h"
#include "tskm_main.h"
#include "tskm_state.h"
#include "tskm_util.h"

#include "tskm_srvr.h"
#include "tskm_port_pf.h"
#include "tskm_port_subsys.h"

/**
 *  _TSKM_SvcCtl Sub-func Task-startup-process
 *  @param  svcId     service id
 *  @param  p_main    TSKM_MAIN_CTX_t*
 *  @param  p_svc     TSKM_SVC_CTX_t*
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */

TSKM_ERR_t tskm_svcExecRequest(TSKM_SVCID_t svcId, TSKM_MAIN_CTX_t* p_main, TSKM_SVC_CTX_t* p_svc) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  // Startup tasks
  TSKM_GSTEP_REQ_INFO_t req = { 0 };

  if (p_main->isOnStartDone == TSKM_FALSE) {
    TSKM_PRINTF(TSKM_LOG_WARN, "Transient SVC cannot exec before get BootInfo");
    return TSKM_E_STATE;
  } else if (p_main->isOnStopDone == TSKM_TRUE) {
    TSKM_PRINTF(TSKM_LOG_WARN, "Transient SVC cannot exec after FrameworkunifiedOnStop");
    return TSKM_E_STATE;
  }

  tskmRet = tskm_svcExec(p_svc);
  TSKM_ERR_CHK_DFT;
  if (p_svc->state == TSKM_SVC_WAITCONNECT) {
    // In the state of waiting to execute
    req.svcId = svcId;
    req.localStep = TSKM_LSTEP_ALL;
    tskmRet = tskm_svcWakeupRequest(p_svc, &req);
    TSKM_ERR_CHK_DFT;  // LCOV_EXCL_BR_LINE 6: Because TSKM_ERR_CHK_DFT does not specify a condition for goto to ERROR
  }
  ERROR: return tskmRet;
}

/**
 *  _TSKM_SvcCtl Sub-func Task-startup-process
 *  @param  p_main    TSKM_MAIN_CTX_t*
 *  @param  svcId     TSKM_SVCID_t
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */

TSKM_ERR_t tskm_svcReserveRequest(TSKM_MAIN_CTX_t* p_main, TSKM_SVCID_t svcId) {
  uint8_t rsvSvcNum = p_main->nvInfo.body.rsvSvcNum;
  TSKM_SVCID_t* p_rsvSvcs = p_main->nvInfo.body.rsvSvcs;
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  if (rsvSvcNum >= TSKM_SVC_RESERVE_MAX) {
    tskmRet = TSKM_E_STATE;
  } else {
    int ii;
    int ret;

    for (ii = 0; ii < rsvSvcNum; ii++) {
      if (p_rsvSvcs[ii] == svcId) {
        TSKM_ASSERT_PRINT(0, "Rsv Req already registered(%d)", svcId);
        // Return OK as continuable
        return tskmRet;
      }
    }

    p_rsvSvcs[rsvSvcNum] = svcId;
    p_main->nvInfo.body.rsvSvcNum++;

    TSKM_PRINTF(TSKM_LOG_STATE, "Rsv Svc registered(%#x)", svcId);

    ret = tskm_pf_nvFileWrite(&p_main->nvInfo);  // LCOV_EXCL_BR_LINE 6: Return value of 0 only
    if (ret == -1) {
      TSKM_ASSERT(0);
      tskmRet = TSKM_E_NG;
      return tskmRet;
    }
  }
  return tskmRet;
}

/**
 *  Service control
 *  @param  svcId     service id
 *  @param  ctl       control command
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */
TSKM_ERR_t _TSKM_SvcCtl(TSKM_SVCID_t svcId, const TSKM_SVC_CTL_t* ctl) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;
  TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();

  TSKM_SVC_CTX_t* p_svc;

  if (ctl == NULL) {
    TSKM_ASSERT(0);
    return TSKM_E_PAR;
  }

  p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs, svcId);
  if (p_svc == NULL) {
    TSKM_ASSERT(0);
    TSKM_PRINTF(TSKM_LOG_ERROR, "unknown svcId:%#x", svcId);
    return TSKM_E_PAR;
  }

  TSKM_PRINTF(TSKM_LOG_API, "%s(%#x,%d)", __FUNCTION__, svcId, ctl->cmd);

  if (ctl->cmd == TSKM_SVC_CMD_EXEC) {
    tskmRet = tskm_svcExecRequest(svcId, p_main, p_svc);
  } else if (ctl->cmd == TSKM_SVC_CMD_ENABLE) {
    tskmRet = tskm_svcEnableRequest(p_svc);
    TSKM_ERR_CHK_DFT;  // LCOV_EXCL_BR_LINE 8: Because TSKM_ERR_CHK_DFT does not specify a condition for goto to ERROR

  } else if (ctl->cmd == TSKM_SVC_CMD_DISABLE) {
    tskmRet = tskm_svcDisableRequest(p_svc);
    TSKM_ERR_CHK_DFT;

  } else if (ctl->cmd == TSKM_SVC_CMD_RESERVE) {
    tskmRet = tskm_svcReserveRequest(p_main, svcId);

  } else {
    return TSKM_E_PAR;
  }

  ERROR: return tskmRet;
}

/**
 *  Get service info
 *  @param  svcId         service id
 *  @param  svcInfo[O]    service info
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */
TSKM_ERR_t TSKM_SvcGetInfo(TSKM_SVCID_t svcId, TSKM_SVC_INFO_t* svcInfo) {
// Do not LOG acquisition APIs  TSKM_PRINTF(TSKM_LOG_API,"%s()",__FUNCTION__)

  TSKM_MAIN_CTX_t* p_main = tskm_getMainCtx();
  TSKM_SVC_CTX_t* p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs, svcId);

  if (p_svc == NULL || svcInfo == NULL) {
    return TSKM_E_PAR;
  }

  svcInfo->svcId = p_svc->attr->svcId;
  svcInfo->isExecDisable =
      (p_svc->state == TSKM_SVC_DISABLE) ? TSKM_TRUE : TSKM_FALSE;

  return TSKM_E_OK;
}

RPC_Result _TSKM_ErrorReboot(const TSKM_ERROR_REBOOT_t* p_info) {
  RPC_Result rpcRet = RPC_OK;
  TSKM_ERROR_REBOOT_t rebootInfo;

  if (!p_info || (p_info->type != TSKM_ERROR_REBOOT_NORMAL)) {
    TSKM_ASSERT(0);
    return RPC_ERR_Fatal;
  }

  memset(&rebootInfo, 0, sizeof(TSKM_ERROR_REBOOT_t));
  rebootInfo.type = p_info->type;
  snprintf(rebootInfo.log.messageStr, TSKM_LOGGING_MSG_STR_SIZE, "%s",
           p_info->log.messageStr);

  TSKM_PRINTF(TSKM_LOG_API, "%s(%d)", __FUNCTION__, p_info->type);

  tskm_sub_reboot(&rebootInfo);

  return rpcRet;
}

/**
 *  Reboot service
 *  @param  p_rsv     Reserved 
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */
TSKM_ERR_t TSKM_Reboot(const TSKM_RSV_t * p_rsv) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  TSKM_PRINTF(TSKM_LOG_API, "%s()", __FUNCTION__)
  if (p_rsv != NULL) {
    TSKM_ASSERT(0);
    return TSKM_E_PAR;
  }

  tskm_sub_reboot_normal();

  return tskmRet;
}

/**
 *  Save LOGs
 *  @param  p_info    LOG storage info
 *  @return TSKM_E_OK Succeeded
 *  @return other     Failed
 */
TSKM_ERR_t TSKM_Logging(const TSKM_LOGGING_INFO_t* p_info) {
  TSKM_LOGGING_INFO_t logInfo;
  TSKM_PRINTF(TSKM_LOG_API, "%s()", __FUNCTION__)

  if (!p_info) {
    TSKM_ASSERT(0);
    return TSKM_E_PAR;
  } else {
    switch (p_info->type) {
      case TSKM_LOGGING_TYPE_MODULE_LOGS:
      case TSKM_LOGGING_TYPE_GRP_RELAUNCH:
        break;
      default:
        TSKM_ASSERT(0);
        return TSKM_E_PAR;
    }
  }

  logInfo.type = p_info->type;
  snprintf(logInfo.messageStr, TSKM_LOGGING_MSG_STR_SIZE, "%s",
           p_info->messageStr);

  tskm_sub_logging(&logInfo);

  return TSKM_E_OK;
}

TSKM_ERR_t TSKM_SetWakeupOrder(const TSKM_WAKEUP_ORDER_t* p_order) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  TSKM_PRINTF(TSKM_LOG_API, "%s()", __FUNCTION__)

  if (p_order == NULL) {
    return TSKM_E_PAR;
  }

  if (strnlen(p_order->orderName, TSKM_ORDER_NAME_MAX) >= TSKM_ORDER_NAME_MAX) {
    return TSKM_E_PAR;
  }

  if (tskm_sub_setWakeupOrder(p_order->orderName) != 0) {
    TSKM_ASSERT(0);
    tskmRet = TSKM_E_NG;
  }

  return tskmRet;
}  // LCOV_EXCL_BR_LINE 10: Final line

