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

#include "tskm_shutdown.h"
#include "tskm_util.h"
#include "tskm_debug.h"
#include "tskm_state.h"

#include "tskm_port_pf.h"
#include "tskm_port_subsys.h"
#include "tskm_gstep.h"


/*********************************************************
 *        Get context of gradual startup
 *********************************************************/
TSKM_STATIC TSKM_GSTEP_CTX_t*
gstepGetDownCtx(TSKM_MAIN_CTX_t* p_main) {
  return &p_main->down;
}

/*********************************************************
 *        System shutdown completed
 *********************************************************/
TSKM_STATIC void downFinish(TSKM_MAIN_CTX_t* p_main) {
  int ret;

  // Send FrameworkunifiedOnStop response
  ret = tskm_pf_sendStopCompResp();
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 8: As ret never becomes non-zero
    // LCOV_EXCL_START 8: As ret never becomes non-zero
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    // LCOV_EXCL_STOP
  }

  // Terminate subsystems
  tskm_sub_term();
}
/*********************************************************
 *        Challenge for a transition to the next state
 *********************************************************/
TSKM_STATIC void tryTransNextState(TSKM_MAIN_CTX_t* p_main) {
  TSKM_GSTEP_CTX_t* p_down;
  TSKM_GSTEP_t* p_current;

  p_down = gstepGetDownCtx(p_main);
  p_current = gstepGetCurrent(p_down);

  if (gstepIsLast(p_down)) {
    TSKM_SVC_WAIT_STATE_t waitState;

    // Checking the ShutdownWaitSVC termination status
    waitState = tskm_svcsGetSvcTermWaitState(&p_main->svcs);
    switch (waitState) {
      case TSKM_SVC_WAIT_BOTH:
        break;
      case TSKM_SVC_WAIT_TRANSIENT:
        if ((p_down->compState & INI_TERMCOMP_RESIDENT) != INI_TERMCOMP_RESIDENT) {
          // Nonresident service termination processing
          if (TSKM_E_OK != tskm_svcsAvtiveSvcTerm(&p_main->svcs)) {  // LCOV_EXCL_BR_LINE 5: tskm_svcsAvtiveSvcTerm is always return TSKM_E_OK
            // LCOV_EXCL_START 5: tskm_svcsAvtiveSvcTerm is always return TSKM_E_OK
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            TSKM_ASSERT(0);
            // LCOV_EXCL_STOP
          }
          // Nonresident SVC terminated and started
          p_down->compState |= INI_TERMCOMP_RESIDENT;
          TSKM_PRINTF(TSKM_LOG_SVCSTATE, "TERM COMP :%s(%#llx)",
                      tskm_convTermCompId2Str(INI_TERMCOMP_RESIDENT),
                      INI_TERMCOMP_RESIDENT);
        }
        break;
      case TSKM_SVC_WAIT_NONE:
        // Complete ShutdownWaitSVC
        // Nonresident SVC terminated and started
        p_down->compState |= INI_TERMCOMP_TRANSIENT;
        TSKM_PRINTF(TSKM_LOG_SVCSTATE, "TERM COMP :%s(%#llx)",
                    tskm_convTermCompId2Str(INI_TERMCOMP_TRANSIENT),
                    INI_TERMCOMP_TRANSIENT);
        break;
      default:
        // LCOV_EXCL_START 8: As it does not meet the default requirements
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        TSKM_ASSERT(0);
        break;
        // LCOV_EXCL_STOP
    }
  }

  if (tskm_svcsIsWaiting(&p_main->svcs) == TSKM_FALSE &&  // No waiting services
      ((p_down->compState & p_current->nextTransCond)
          == p_current->nextTransCond)) {  // Event completion condition
    if (gstepIsLast(p_down)) {
      tskm_stateTransit(p_main, TSKM_ST_ACCON, TSKM_ST_ACCOFF);
    } else {
      tskm_stateTransit(p_main, TSKM_ST_DOWN, TSKM_ST_DOWN);
    }
  }
}

/*********************************************************
 *        Check and process gradual termination request
 *********************************************************/
TSKM_ERR_t tskm_entryDown_Check_Req(TSKM_MAIN_CTX_t* p_main,
                                    TSKM_GSTEP_t* p_current) {
  uint32_t ii;

  if (0 == p_current->reqNum) {
    EFrameworkunifiedStatus l_eStatus;

    // If there is no gradual termination request, check the transition to the next step.
    l_eStatus = FrameworkunifiedSendSelf(p_main->hApp, TSKM_TRANS_STEP_REQ, 0, NULL);
    if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 5: Checked in death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      return TSKM_E_NG;
      // LCOV_EXCL_STOP
    }

  } else {
    // Gradual termination request
    for (ii = 0; ii < p_current->reqNum; ii++) {
      TSKM_ERR_t tskmRet;
      TSKM_GSTEP_REQ_INFO_t* p_req = &p_current->reqList[ii];
      TSKM_SVC_CTX_t* p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs,
                                                     p_req->svcId);

      tskmRet = tskm_svcDownRequest(p_svc, p_req);
      if (tskmRet != TSKM_E_OK) {
        TSKM_ASSERT(0);
      }
    }
  }
  return TSKM_E_OK;
}

/*********************************************************
 *        Gradual termination entry process
 *********************************************************/
TSKM_ERR_t tskm_entryDown(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();

  TSKM_GSTEP_CTX_t* p_down;
  TSKM_GSTEP_t* p_current;

  p_main->state = TSKM_ST_DOWN;
  p_down = gstepGetDownCtx(p_main);
  p_current = gstepGetCurrent(p_down);
  if (p_current == NULL) {  // LCOV_EXCL_BR_LINE 200:p_current can not be null
    // LCOV_EXCL_START 200:p_current can not be null
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  TSKM_PRINTF(TSKM_LOG_SVCSTATE, "DOWN GSTEP:%d", p_down->gstepIdx);

  if (tskm_entryDown_Check_Req(p_main, p_current) == TSKM_E_NG) {  // LCOV_EXCL_BR_LINE 200:the function of tskm_entryDown_Check_Req can not be TSKM_E_NG at this case // NOLINT(whitespace/line_length)
    // LCOV_EXCL_START 200:tskm_entryDown_Check_Req can not be TSKM_E_NG at this case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (p_current->nextTransCond) {
    TSKM_PRINTF(TSKM_LOG_STATE, "WAIT COMP:%s(%llx)",
                tskm_convTermCompId2Str(p_current->nextTransCond),
                p_current->nextTransCond);
  }

  TSKM_FUNC_OUT();
  return TSKM_E_OK;

  ERROR: tskm_pf_abort();
  return TSKM_E_NG;
}

/*********************************************************
 *        Gradual termination exit process
 *********************************************************/
TSKM_ERR_t tskm_exitDown(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();
  TSKM_GSTEP_CTX_t* p_down;

  p_down = gstepGetDownCtx(p_main);

  if (gstepIsLast(p_down)) {
    TSKM_PRINTF(TSKM_LOG_STATE, "DOWN FINISH");
    downFinish(p_main);    // Final process of termination
  }

  changeNextStep(p_down);  // Proceed to the next step

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}
/*********************************************************
 *        Hander during gradual termination
 *********************************************************/
TSKM_ERR_t tskm_handleDown(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev) {
  TSKM_FUNC_IN();

  switch (p_ev->event) {
    case TSKM_EV_PRI_REP_DOWN_COMP:  // Notificaton for event completion in termination
    {
      TSKM_PRINTF(TSKM_LOG_SVCSTATE, "TERM COMP :%s(%#llx) from:%d",
                  tskm_convTermCompId2Str(p_ev->prm.repDownComp.compId),
                  p_ev->prm.repDownComp.compId, p_ev->fromPid);
      TSKM_GSTEP_CTX_t* p_down = gstepGetDownCtx(p_main);
      p_down->compState |= p_ev->prm.repDownComp.compId;
      tryTransNextState(p_main);
    }
      break;
    case TSKM_EV_LCL_CHG_SVC_STATE:  // Service state change
      tryTransNextState(p_main);
      break;
    case TSKM_EV_LCL_REQ_TRANS_STEP:  //Step transition request
      tryTransNextState(p_main);
      break;
    default:
      tskm_handleAccon(p_main, p_ev);
      break;
  }
  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}  // LCOV_EXCL_BR_LINE 10: Final line

