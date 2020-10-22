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

#include "tskm_wakeup.h"
#include "tskm_debug.h"
#include "tskm_util.h"
#include "tskm_state.h"
#include "tskm_port_subsys.h"
#include "tskm_port_pf.h"

#include "tskm_gstep.h"


/*********************************************************
 *        Get gradual startup context
 *********************************************************/
TSKM_STATIC TSKM_GSTEP_CTX_t*
gstepGetWakeupCtx(TSKM_MAIN_CTX_t* p_main) {
  return &p_main->wakeup;
}

/***********************************************************************
 *        Startup completion process
 ***********************************************************************/
TSKM_STATIC void wakeupFinish(TSKM_MAIN_CTX_t* p_main) {
  return;
}

/*********************************************************
 *        Challenge for a transition to the next state
 *********************************************************/
TSKM_STATIC void tryTransNextState(TSKM_MAIN_CTX_t* p_main) {
  TSKM_GSTEP_CTX_t* p_wakeup;
  TSKM_GSTEP_t* p_current;

  p_wakeup = gstepGetWakeupCtx(p_main);
  p_current = gstepGetCurrent(p_wakeup);

  if (!p_current) {
    return;
  }

  // LCOV_EXCL_BR_START 8: Because the second condition in the if statement is never false
  if (tskm_svcsIsWaiting(&p_main->svcs) == TSKM_FALSE &&  // No waiting services
      ((p_wakeup->compState & p_current->nextTransCond)
          == p_current->nextTransCond)) {  // Event completion condition
    // LCOV_EXCL_BR_STOP
    if (gstepIsLast(p_wakeup)) {
      tskm_stateTransit(p_main, TSKM_ST_WAKEUP, TSKM_ST_RUNNING);
    } else {
      tskm_stateTransit(p_main, TSKM_ST_WAKEUP, TSKM_ST_WAKEUP);
    }
  }
}

/*********************************************************
 *        Gradual startup request issuance process
 *********************************************************/
TSKM_ERR_t tskm_entryWakeup_Req(TSKM_MAIN_CTX_t* p_main,
                                TSKM_GSTEP_t* p_current) {
  uint32_t ii;

  // Refer to the stepId and perform preprocessing if needed.
  if (p_current->gstepId == TSKM_GSTEP_BUPCHK) {
    TSKM_PRINTF(TSKM_LOG_STATE, "BUPCHK EXE");
    // When TaskManager is used as a system Launcher, system data is initialized here.
  }

  // Start process
  for (ii = 0; ii < p_current->execSvcNum; ii++) {
    TSKM_ERR_t tskmRet;
    TSKM_SVCID_t svcId = p_current->execSvcIdList[ii];
    TSKM_SVC_CTX_t* p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs, svcId);

    tskmRet = tskm_svcExec(p_svc);
    TSKM_ERR_CHK_DFT;
  }

  // Issue gradual startup request
  for (ii = 0; ii < p_current->reqNum; ii++) {
    TSKM_ERR_t tskmRet;
    TSKM_GSTEP_REQ_INFO_t* p_req = &p_current->reqList[ii];
    TSKM_SVC_CTX_t* p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs, p_req->svcId);

    // Queuing in the SVC layer even for services that are not started.
    tskmRet = tskm_svcWakeupRequest(p_svc, p_req);
    TSKM_ERR_CHK_DFT;  // LCOV_EXCL_BR_LINE 6: Because TSKM_ERR_CHK_DFT does not specify a condition for goto to ERROR
  }

  if (p_current->nextTransCond) {
    TSKM_PRINTF(TSKM_LOG_STATE, "WAIT COMP:%s(%llx)",
                tskm_convInitCompId2Str(p_current->nextTransCond),
                p_current->nextTransCond);
  }

  return TSKM_E_OK;

  // LCOV_EXCL_START 6: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return TSKM_E_NG;
  // LCOV_EXCL_STOP
}

/*********************************************************
 *        Gradual startup entry process
 *********************************************************/
TSKM_ERR_t tskm_entryWakeup(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();

  TSKM_GSTEP_CTX_t* p_wakeup;
  TSKM_GSTEP_t* p_current;

  p_main->state = TSKM_ST_WAKEUP;
  p_wakeup = gstepGetWakeupCtx(p_main);
  p_current = gstepGetCurrent(p_wakeup);

  if (p_current == NULL) {  // LCOV_EXCL_BR_LINE 8: Because condition false setting is not possible
    // LCOV_EXCL_START 8: Because condition false setting is not possible
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    tskm_pf_exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP
  }

  TSKM_PRINTF(TSKM_LOG_SVCSTATE, "WAKEUP GSTEP:%d", p_wakeup->gstepIdx);

  if (tskm_entryWakeup_Req(p_main, p_current) == TSKM_E_NG) {  // LCOV_EXCL_BR_LINE 200:the function of tskm_entryWakeup_Req can not be TSKM_E_NG at this case // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 200:the function of tskm_entryWakeup_Req can not be TSKM_E_NG at this case // NOLINT(whitespace/line_length)
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    tskm_pf_exit(EXIT_FAILURE);
    // LCOV_EXCL_STOP
  }

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*********************************************************
 *        Gradual startup exit process
 *********************************************************/
TSKM_ERR_t tskm_exitWakeup(TSKM_MAIN_CTX_t* p_main) {
  TSKM_GSTEP_CTX_t* p_wakeup;

  TSKM_FUNC_IN();
  p_wakeup = gstepGetWakeupCtx(p_main);

  // Exit process at the end of the gradual startup
  if (gstepIsLast(p_wakeup)) {
    TSKM_PRINTF(TSKM_LOG_STATE, "WAKEUP FIN");
    wakeupFinish(p_main);
  }

  changeNextStep(p_wakeup);  // Transition to next step

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*********************************************************
 *        Gradual startup handler
 *********************************************************/
TSKM_ERR_t tskm_handleWakeup(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev) {
  TSKM_FUNC_IN();
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  switch (p_ev->event) {
    case TSKM_EV_PRI_REP_WAKEUP_COMP:  // Event completion notification at startup
    {
      TSKM_PRINTF(TSKM_LOG_SVCSTATE, "INIT COMP :%s(%#llx) from:%d",
                  tskm_convInitCompId2Str(p_ev->prm.repWakeupComp.compId),
                  p_ev->prm.repWakeupComp.compId, p_ev->fromPid);
      TSKM_GSTEP_CTX_t* p_wakeup = gstepGetWakeupCtx(p_main);
      p_wakeup->compState |= p_ev->prm.repWakeupComp.compId;
      tryTransNextState(p_main);
    }
      break;
    case TSKM_EV_LCL_CHG_SVC_STATE:  // Service state change
      tryTransNextState(p_main);
      break;
    default:
      tskmRet = tskm_handleAccon(p_main, p_ev);
      break;
  }
  TSKM_FUNC_OUT();
  return tskmRet;
}  // LCOV_EXCL_BR_LINE 10: Final line

