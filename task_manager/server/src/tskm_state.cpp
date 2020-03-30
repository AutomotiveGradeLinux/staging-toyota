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

#include "tskm_state.h"
#include <stdlib.h>
#include "tskm_util.h"
#include "tskm_debug.h"
#include "tskm_wakeup.h"
#include "tskm_shutdown.h"
#include "tskm_port_subsys.h"
#include "tskm_port_pf.h"



// Prototype declarations
TSKM_ERR_t tskm_entryAccoff(TSKM_MAIN_CTX_t* p_main);
TSKM_ERR_t tskm_exitAccoff(TSKM_MAIN_CTX_t* p_main);
TSKM_ERR_t tskm_handleAccoff(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev);

TSKM_ERR_t tskm_entryAccon(TSKM_MAIN_CTX_t* p_main);
TSKM_ERR_t tskm_exitAccon(TSKM_MAIN_CTX_t* p_main);

TSKM_ERR_t tskm_entryRunning(TSKM_MAIN_CTX_t* p_main);
TSKM_ERR_t tskm_exitRunning(TSKM_MAIN_CTX_t* p_main);
TSKM_ERR_t tskm_handleRunning(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev);

// Structures of state transitioning callback functions
typedef TSKM_ERR_t (*entry_state_t)(TSKM_MAIN_CTX_t* p_main);
typedef TSKM_ERR_t (*exit_state_t)(TSKM_MAIN_CTX_t* p_main);
typedef TSKM_ERR_t (*event_handler_t)(TSKM_MAIN_CTX_t* p_main,
                                      TSKM_EVENT_INFO_t* p_ev);

typedef struct {
  TSKM_STATE_t state;
  entry_state_t entry_func;
  exit_state_t exit_func;
  event_handler_t event_handler;
} state_func_table_t;

// State specific function table
static const state_func_table_t state_func_table[] = { { TSKM_ST_ACCOFF,
    tskm_entryAccoff, tskm_exitAccoff, tskm_handleAccoff }, { TSKM_ST_ACCON,
    tskm_entryAccon, tskm_exitAccon, tskm_handleAccon }, { TSKM_ST_WAKEUP,
    tskm_entryWakeup, tskm_exitWakeup, tskm_handleWakeup }, { TSKM_ST_RUNNING,
    tskm_entryRunning, tskm_exitRunning, tskm_handleRunning }, { TSKM_ST_DOWN,
    tskm_entryDown, tskm_exitDown, tskm_handleDown }, { 0, 0, 0, 0 } };

/****************************************************
 *        ACC OFF ENTRY
 ****************************************************/
TSKM_ERR_t tskm_entryAccoff(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();
  p_main->state = TSKM_ST_ACCOFF;
  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *              ACC OFF EXIT
 *************************************************************************/
TSKM_ERR_t tskm_exitAccoff(TSKM_MAIN_CTX_t* p_main) {
  // Do nothing
  // Called only once at startup
  return TSKM_E_OK;
}

/*************************************************************************
 *              ACC OFF HANDLE
 *************************************************************************/
TSKM_ERR_t tskm_handleAccoff(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev) {
  TSKM_ASSERT(0);  // Do nothing
  return TSKM_E_OK;
}

/*************************************************************************
 *              ACC ON ENTRY
 *************************************************************************/
TSKM_ERR_t tskm_entryAccon(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();
  p_main->state = TSKM_ST_ACCON;

  tskm_entryState(p_main, TSKM_ST_WAKEUP);
  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *              ACC ON HANDLE
 *************************************************************************/
TSKM_ERR_t tskm_exitAccon(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();

  TSKM_ERR_t tskmRet = TSKM_E_OK;

  if (TSKM_SUB_STATE_IS(p_main, TSKM_ST_WAKEUP)) {  // LCOV_EXCL_BR_LINE 8:Because the condition is never true
    // LCOV_EXCL_START 8: Because the condition is never true
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    tskmRet = tskm_exitState(p_main, TSKM_ST_WAKEUP);
    // LCOV_EXCL_STOP
  } else if (TSKM_SUB_STATE_IS(p_main, TSKM_ST_RUNNING)) {  // LCOV_EXCL_BR_LINE 8: Because the condition is never true
    // LCOV_EXCL_START 8: Because the condition is never true
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    tskmRet = tskm_exitState(p_main, TSKM_ST_RUNNING);
    // LCOV_EXCL_STOP
  } else if (TSKM_SUB_STATE_IS(p_main, TSKM_ST_DOWN)) {  // LCOV_EXCL_BR_LINE 8:Because the condition is never false
    tskmRet = tskm_exitState(p_main, TSKM_ST_DOWN);
  }

  TSKM_ERR_CHK(tskmRet, TSKM_E_OK, ERROR);  // LCOV_EXCL_BR_LINE 8: Because the tskmRet does not change to NG
  p_main->isExec = TSKM_FALSE;
  ERROR:
  TSKM_FUNC_OUT();
  return tskmRet;
}

/*************************************************************************
 *              POLL EVENT HANDLE
 *************************************************************************/
TSKM_STATIC void checkHungSvcs(TSKM_MAIN_CTX_t* p_main) {
  int ret;
  TSKM_HUNG_INFO_t *p_hungSvcList = NULL;

  p_hungSvcList = tskm_sub_searchHungSvcs();
  if (p_hungSvcList != NULL) {
    int hungSvcNum = 0;

    while (p_hungSvcList[hungSvcNum].pid != -1) {
      pid_t pid = p_hungSvcList[hungSvcNum].pid;

      TSKM_SVC_CTX_t* p_svc = tskm_svcsGetSvcByPid(&p_main->svcs, pid);

      if (p_svc) {
        TSKM_PRINTF(TSKM_LOG_SYSTEMDATA, "HUNG SVC(%s:%d), TYPE(%d)",
                    p_svc->attr->name, pid, p_hungSvcList[hungSvcNum].type);

        ret = tskm_pf_terminateProcGroup(static_cast<uint16_t>(pid));
        if (ret != 0) {
          TSKM_ASSERT(0);
        }

      } else {
        TSKM_PRINTF(TSKM_LOG_WARN, "UNKNOWN HUNG SVC(%d), TYPE(%d)", pid,
                    p_hungSvcList[hungSvcNum].type);
      }

      hungSvcNum++;
    }

    free(p_hungSvcList);
  }

  return;
}

#define AVAILABILITY_CHECK_RETRY_COUNT 24

/*************************************************************************
 *              CHECK SVC AVAILABILITY
 *************************************************************************/
TSKM_STATIC void checkSvcAvailability(TSKM_MAIN_CTX_t* p_main) {
  for (uint32_t ii = 0; ii < p_main->svcs.svcNum; ii++) {
    if (p_main->svcs.svcList[ii].state == TSKM_SVC_RUNNING
        && !p_main->svcs.svcList[ii].isAvailable) {
      p_main->svcs.svcList[ii].watchCnt++;
      TSKM_ASSERT_PRINT(0, "WAIT AVAILABILITY FOR %s(%d) (%d/%d)",
                        p_main->svcs.svcList[ii].attr->name,
                        p_main->svcs.svcList[ii].pid,
                        p_main->svcs.svcList[ii].watchCnt,
                        AVAILABILITY_CHECK_RETRY_COUNT);

      if (p_main->svcs.svcList[ii].watchCnt > AVAILABILITY_CHECK_RETRY_COUNT) {
        int ret;

        ret = tskm_pf_terminateProcGroup(static_cast<uint16_t>(p_main->svcs.svcList[ii].pid));
        if (ret != 0) {
          TSKM_ASSERT(0);
        }
      }
    }
  }

  return;
}

/*************************************************************************
 *              POLL EVENT HANDLE
 *************************************************************************/
TSKM_STATIC void handlePolling(TSKM_MAIN_CTX_t* p_main) {
  checkHungSvcs(p_main);

  checkSvcAvailability(p_main);

  return;
}

/*************************************************************************
 *              LOW MEMORY EVENT HANDLE
 *************************************************************************/
TSKM_STATIC void handleLowMem(TSKM_MAIN_CTX_t* p_main) {
  TSKM_ERR_t tskmRet = TSKM_E_OK;

  tskmRet = tskm_svcsCallLowMem(&p_main->svcs);
  if (TSKM_E_OK != tskmRet) {
    TSKM_ASSERT(0);
  }

  return;
}
/*************************************************************************
 *              ACC ON HANDLE
 *************************************************************************/
TSKM_ERR_t tskm_handleAccon(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev) {
  TSKM_FUNC_IN();

  switch (p_ev->event) {
    case TSKM_EV_LCL_REQ_SDUMP:
      tskm_svcsDump(&p_main->svcs);
      break;
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REP_POLLING:
      TSKM_PRINTF(TSKM_LOG_DEBUG, "watch timer polling event.");
      handlePolling(p_main);
      break;
    default:
      TSKM_PRINTF(TSKM_LOG_STATE, "IGNORE:%s(%d)",
                  tskm_convEvent2Str(p_ev->event), p_ev->event);
      break;
  }
  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *              BOOT RESERVED SERVICES
 *************************************************************************/
static int bootRsvSvcs(TSKM_MAIN_CTX_t* p_main) {
  uint32_t ii;
  TSKM_ERR_t tskmRet = TSKM_E_OK;
  uint8_t rsvSvcNum = p_main->nvInfo.body.rsvSvcNum;
  TSKM_SVCID_t* p_rsvSvcs = p_main->nvInfo.body.rsvSvcs;

  TSKM_PRINTF(TSKM_LOG_STATE, "RSV SVC NUM = %d", rsvSvcNum);

  for (ii = 0; ii < rsvSvcNum; ii++) {
    TSKM_GSTEP_REQ_INFO_t req = { 0 };
    TSKM_SVC_CTX_t* p_svc;

    p_svc = tskm_svcsGetSvcBySvcId(&p_main->svcs, p_rsvSvcs[ii]);
    if (p_svc == NULL) {
      TSKM_ASSERT(0);
      continue;
    }

    tskmRet = tskm_svcExec(p_svc);
    if (TSKM_E_OK != tskmRet) {
      TSKM_ASSERT(0);
      continue;
    }
    if (p_svc->state == TSKM_SVC_WAITCONNECT) {
      // In the state waiting for execution
      req.svcId = p_rsvSvcs[ii];
      req.localStep = TSKM_LSTEP_ALL;
      tskmRet = tskm_svcWakeupRequest(p_svc, &req);
      if (TSKM_E_OK != tskmRet) {  // LCOV_EXCL_BR_LINE 8: Because the condition is never true
        // LCOV_EXCL_START 8: Because the condition is never true
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        TSKM_ASSERT_PRINT(0, "tskmRet = %d", tskmRet);
        continue;
        // LCOV_EXCL_STOP
      }
    }
  }

  if (rsvSvcNum != 0) {
    int ret;

    for (ii = 0; ii < TSKM_SVC_RESERVE_MAX; ii++) {
      p_rsvSvcs[ii] = TSKM_SVCID_NONE;
    }

    p_main->nvInfo.body.rsvSvcNum = 0;

    ret = tskm_pf_nvFileWrite(&p_main->nvInfo);
    if (ret == -1) {  // LCOV_EXCL_BR_LINE 8: Because the tskm_pf_nvFileWrite() only returns a return value of 0
      // LCOV_EXCL_START 8:Because the condition is never true
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }
  }

  return 0;
  // LCOV_EXCL_START 8: Because the tskm_pf_nvFileWrite() only returns a return value of 0
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return -1;
  // LCOV_EXCL_STOP
}

/*************************************************************************
 *              RUN ENTRY
 *************************************************************************/
TSKM_ERR_t tskm_entryRunning(TSKM_MAIN_CTX_t* p_main) {
  int ret;

  TSKM_FUNC_IN();
  p_main->state = TSKM_ST_RUNNING;

  ret = bootRsvSvcs(p_main);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 8: Because bootRsvSvcs returns only a return value of 0
    // LCOV_EXCL_START 8: Because bootRsvSvcs returns only a return value of 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    // LCOV_EXCL_STOP
  }

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *              RUN EXIT
 *************************************************************************/
TSKM_ERR_t tskm_exitRunning(TSKM_MAIN_CTX_t* p_main) {
  TSKM_FUNC_IN();

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *              RUN HANDLE
 *************************************************************************/
TSKM_ERR_t tskm_handleRunning(TSKM_MAIN_CTX_t* p_main,
                              TSKM_EVENT_INFO_t* p_ev) {
  TSKM_FUNC_IN();

  switch (p_ev->event) {
    case TSKM_EV_LCL_REQ_STOP:
      tskm_stateTransit(p_main, TSKM_ST_RUNNING, TSKM_ST_DOWN);
      break;
    case TSKM_EV_LCL_REP_LOWMEM:
      handleLowMem(p_main);
      break;
    default:
      tskm_handleAccon(p_main, p_ev);
      break;
  }

  TSKM_FUNC_OUT();
  return TSKM_E_OK;
}

/*************************************************************************
 *                Get transition table
 *************************************************************************/
static const state_func_table_t*
tskm_getFuncTable(TSKM_STATE_t state) {
  int i;
  for (i = 0; state_func_table[i].state != 0; i++) {
    if (state == state_func_table[i].state) {
      return &state_func_table[i];
    }
  }
  TSKM_ASSERT(0);
  return &state_func_table[0];
}

/*************************************************************************
 *                State transition instructions
 *************************************************************************/
TSKM_ERR_t tskm_stateTransit(TSKM_MAIN_CTX_t* p_main, TSKM_STATE_t srcState,
                             TSKM_STATE_t dstState) {
  TSKM_ERR_t tskmRet;
  TSKM_PRINTF(TSKM_LOG_STATE, "STATE:%s(%s) -> %s",
              tskm_convState2Str(srcState), tskm_convState2Str(p_main->state),
              tskm_convState2Str(dstState));

  tskmRet = tskm_exitState(p_main, srcState);
  TSKM_ERR_CHK(tskmRet, TSKM_E_OK, ERROR);  // LCOV_EXCL_BR_LINE 8: Because TSKM_ERR_CHK does not specify a goto ERROR condition
  tskmRet = tskm_entryState(p_main, dstState);
  TSKM_ERR_CHK(tskmRet, TSKM_E_OK, ERROR);  // LCOV_EXCL_BR_LINE 8: Because TSKM_ERR_CHK does not specify a goto ERROR condition
  return TSKM_E_OK;

  // LCOV_EXCL_START 8: Because TSKM_ERR_CHK does not specify a goto ERROR condition
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return TSKM_E_NG;
  // LCOV_EXCL_STOP
}

/*************************************************************************
 *                Event handler
 *************************************************************************/
void tskm_handleEvent(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev) {
  event_handler_t handlerFunc;
  handlerFunc = tskm_getFuncTable(p_main->state)->event_handler;
  handlerFunc(p_main, p_ev);
}

/****************************************************
 *        State transitioning entry process
 ****************************************************/
TSKM_ERR_t tskm_entryState(TSKM_MAIN_CTX_t* p_rec, TSKM_STATE_t state) {
  TSKM_ERR_t ret = TSKM_E_NG;
  const state_func_table_t* p_table = tskm_getFuncTable(state);

  TSKM_PRINTF(TSKM_LOG_DEBUG, "entry :%s", tskm_convState2Str(state));
  if (p_table->entry_func) {  // LCOV_EXCL_BR_LINE 8: Because p_table->entry_func never becomes 0
    ret = ((*p_table->entry_func)(p_rec));
  } else {
    // LCOV_EXCL_START 8: Because p_table->entry_func never becomes 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    // LCOV_EXCL_STOP
  }
  return ret;
}

/****************************************************
 *        State transitioning exit process
 ****************************************************/
TSKM_ERR_t tskm_exitState(TSKM_MAIN_CTX_t* p_rec, TSKM_STATE_t state) {
  TSKM_ERR_t ret = TSKM_E_NG;
  const state_func_table_t* p_table = tskm_getFuncTable(state);

  if (p_table->exit_func) {  // LCOV_EXCL_BR_LINE 8: Because p_table->exit_func never becomes 0
    ret = (*p_table->exit_func)(p_rec);
  } else {
    // LCOV_EXCL_START 8: Because p_table->exit_func never becomes 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    // LCOV_EXCL_STOP
  }
  TSKM_PRINTF(TSKM_LOG_DEBUG, "exit :%s", tskm_convState2Str(state));
  return ret;
}  // LCOV_EXCL_BR_LINE 10: Final line

