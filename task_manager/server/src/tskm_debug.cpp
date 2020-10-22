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

const char*
tskm_convState2Str(TSKM_STATE_t state) {
  const char* ret = "UNKNONW";
  switch (state) {    // LCOV_EXCL_BR_LINE 7:debug code
    case TSKM_ST_ACCOFF:
      ret = "ACCOFF";
      break;
    case TSKM_ST_ACCON:
      ret = "ACCON";
      break;
    case TSKM_ST_WAKEUP:
      ret = "WAKEUP";
      break;
    case TSKM_ST_RUNNING:
      ret = "RUNNING";
      break;
    case TSKM_ST_DOWN:
      ret = "DOWN";
      break;
  }
  return ret;
}

const char*
tskm_convEvent2Str(TSKM_EVENT_t event) {
  switch (event) {    // LCOV_EXCL_BR_LINE 7:debug code
    case TSKM_EV_NOP:
      return "NOP";
    case TSKM_EV_PRI_REQ_WAKEUP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REQ_WAKEUP";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REQ_DOWN:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REQ_DOWN";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REQ_TOUCH:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REQ_TOUCH";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REQ_DEBUGDUMP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REQ_DEBUGDUMP";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REP_LOWMEM:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REP_LOWMEM";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REP_WAKEUP_COMP:
      return "PRI_REP_WAKEUP_COMP";
    case TSKM_EV_PRI_REP_DOWN_COMP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REP_DOWN_COMP";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REP_CONNECT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REP_CONNECT";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REP_DISCONNECT:
      return "PRI_REP_DISCONNECT";
    case TSKM_EV_PRI_RES_WAKEUP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_RES_WAKEUP";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_RES_DOWN:
      return "PRI_RES_DOWN";
    case TSKM_EV_PRI_RES_DEBUGDUMP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_RES_DEBUGDUMP";
      // LCOV_EXCL_STOP
    case TSKM_EV_PRI_REQ_EXIT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "PRI_REQ_EXIT";
      // LCOV_EXCL_STOP
    case TSKM_EV_SVC_REP_TERM:
      return "SVC_REP_TERM";
    case TSKM_EV_API_REQ_REBOOT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "API_REQ_REBOOT";
      // LCOV_EXCL_STOP
    case TSKM_EV_API_REQ_SVC_CTL:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "API_REQ_SVC_CTL";
      // LCOV_EXCL_STOP
    case TSKM_EV_API_REQ_SVC_DISABLE:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "API_REQ_SVC_DISABLE";
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REQ_STOP:
      return "API_REQ_STOP";
    case TSKM_EV_LCL_CHG_SVC_STATE:
      return "LCL_CHG_SVC_STATE";
    case TSKM_EV_LCL_REQ_SDUMP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "LCL_REQ_SDUMP";
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REP_TIMEOUT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "LCL_REP_TIMEOUT";
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REP_POLLING:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "LCL_REP_POLLING";
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REP_LOWMEM:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "LCL_REP_LOWMEM";
      // LCOV_EXCL_STOP
    case TSKM_EV_LCL_REQ_TRANS_STEP:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "LCL_REQ_TRANS_STEP";
      // LCOV_EXCL_STOP
    default:
      break;
  }
  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return "UNKNOWN";
  // LCOV_EXCL_STOP
}

const char*
tskm_convInitCompId2Str(uint64_t compId) {
  switch (compId) {    // LCOV_EXCL_BR_LINE 7:debug code
    case INI_INITCOMP_NONE:
      return "NONE";
    case INI_INITCOMP_ON_START:
      return "ONSTART";
    case INI_INITCOMP_NVM_ACCESS:
      return "NVM_ACCESS";
    case INI_INITCOMP_TEST0:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST0";
      // LCOV_EXCL_STOP
    case INI_INITCOMP_TEST1:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST1";
      // LCOV_EXCL_STOP
    case INI_INITCOMP_TEST2:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST2";
      // LCOV_EXCL_STOP
    case INI_INITCOMP_TEST3:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST3";
      // LCOV_EXCL_STOP
  }
  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return "UNKNONW";
  // LCOV_EXCL_STOP
}

const char*
tskm_convTermCompId2Str(uint64_t compId) {
  switch (compId) {  // LCOV_EXCL_BR_LINE 7:debug code
    case INI_TERMCOMP_NONE:
      return "NONE";
    case INI_TERMCOMP_ACTIVITYMGR:
      return "ACTIVITYMGR";
    case INI_TERMCOMP_RESIDENT:
      return "RESIDENT";
    case INI_TERMCOMP_TRANSIENT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TRANSIENT";
      // LCOV_EXCL_STOP
    case INI_TERMCOMP_TEST0:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST0";
      // LCOV_EXCL_STOP
    case INI_TERMCOMP_TEST1:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST1";
      // LCOV_EXCL_STOP
    case INI_TERMCOMP_TEST2:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST2";
      // LCOV_EXCL_STOP
    case INI_TERMCOMP_TEST3:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "TEST3";
      // LCOV_EXCL_STOP
  }
  return "UNKNONW";
}

const char*
tskm_convSvcState2Str(TSKM_SVC_STATE_t state) {
  switch (state) {  // LCOV_EXCL_BR_LINE 7:debug code
    case TSKM_SVC_DORMANT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "DORMANT";
      // LCOV_EXCL_STOP
    case TSKM_SVC_WAITCONNECT:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "WAITCONNECT";
      // LCOV_EXCL_STOP
    case TSKM_SVC_WAKEUP:
      return "WAKEUP";
    case TSKM_SVC_RUNNING:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "RUNNING";
      // LCOV_EXCL_STOP
    case TSKM_SVC_DOWN:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "DOWN";
      // LCOV_EXCL_STOP
    case TSKM_SVC_FINDOWN:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "FINDOWN";
      // LCOV_EXCL_STOP
    case TSKM_SVC_DISABLE:
      // LCOV_EXCL_START 7:debug code
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return "DISABLE";
      // LCOV_EXCL_STOP
    default:
      break;
  }
  return "UNKNONW";
}

TSKM_STATIC void svcDump(TSKM_SVC_CTX_t* p_svc) {
  TSKM_PRINTF(TSKM_LOG_STATE, "<%s>", p_svc->attr->name);
  TSKM_PRINTF(TSKM_LOG_STATE, "  pid:%d svcId:%#x state:%s waitResCnt:%d",
              p_svc->pid, p_svc->attr->svcId,
              tskm_convSvcState2Str(p_svc->state), p_svc->waitResCnt);
}

void tskm_svcsDump(TSKM_SVCS_CTX_t* p_svcs) {
  uint32_t ii;
  for (ii = 0; ii < p_svcs->svcNum; ii++) {
    svcDump(&p_svcs->svcList[ii]);
  }
}

