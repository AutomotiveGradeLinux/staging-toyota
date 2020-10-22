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

#ifndef TASK_MANAGER_SERVER_INCLUDE_TSKM_STATE_H_
#define TASK_MANAGER_SERVER_INCLUDE_TSKM_STATE_H_

#include "system_service/tskm_local_type.h"
#include "system_service/tskm_type.h"
#include "tskm_main.h"

#define TSKM_SUB_STATE_IS(p_main, subState) \
    (((p_main)->state & subState) == subState)  // NOLINT (runtime/references)

// Public functions
TSKM_ERR_t tskm_entryState(TSKM_MAIN_CTX_t* p_rec, TSKM_STATE_t state);
TSKM_ERR_t tskm_exitState(TSKM_MAIN_CTX_t* p_rec, TSKM_STATE_t state);
TSKM_ERR_t tskm_stateTransit(TSKM_MAIN_CTX_t* p_main, TSKM_STATE_t srcState,
                             TSKM_STATE_t dstState);
void tskm_handleEvent(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev);

TSKM_ERR_t tskm_handleAccon(TSKM_MAIN_CTX_t* p_main, TSKM_EVENT_INFO_t* p_ev);

#endif  // TASK_MANAGER_SERVER_INCLUDE_TSKM_STATE_H_
