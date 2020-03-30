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

#ifndef TASK_MANAGER_INCLUDE_SHARE_TSKM_GSTEP_H_
#define TASK_MANAGER_INCLUDE_SHARE_TSKM_GSTEP_H_

#include  "system_service/tskm_local_type.h"
#include  "tskm_debug.h"

/*********************************************************
 *        Get gradual startup current info
 *********************************************************/
inline TSKM_STATIC TSKM_BOOL_t gstepIsLast(TSKM_GSTEP_CTX_t* p_gstep) {
  if (p_gstep->gstepNum == p_gstep->gstepIdx + 1) {
    return TSKM_TRUE;
  }
  return TSKM_FALSE;
}

/*********************************************************
 *        Get gradual startup current info
 *********************************************************/
inline TSKM_STATIC TSKM_GSTEP_t*
gstepGetCurrent(TSKM_GSTEP_CTX_t* p_gstep) {
  if (p_gstep->gstepNum > p_gstep->gstepIdx) {
    return &p_gstep->gstep[p_gstep->gstepIdx];
  }
  return NULL;
}

/*********************************************************
 *        Issue requests waiting for exec completion
 *********************************************************/
inline TSKM_STATIC TSKM_GSTEP_t*
changeNextStep(TSKM_GSTEP_CTX_t* p_gstep) {
  p_gstep->gstepIdx++;
  return gstepGetCurrent(p_gstep);
}

#endif  // TASK_MANAGER_INCLUDE_SHARE_TSKM_GSTEP_H_

