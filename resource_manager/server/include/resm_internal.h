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

#ifndef RESOURCE_MANAGER_SERVER_INCLUDE_RESM_INTERNAL_H_
#define RESOURCE_MANAGER_SERVER_INCLUDE_RESM_INTERNAL_H_

#include <agldd/moduleid.h>
#include <agldd/ev_common.h>

#include <other_service/ev_lib.h>
#include "system_service/resm_type.h"

#define Resm_Flag_ID_Base EV_Flag_ID_Base(RESMGR_MID)

/*******************************************************************************
 * User defined
 *******************************************************************************/
#define MY_MAX(a, b) ((a) > (b) ? (a) : (b))

/***********************************************************
 * Internal API definitions
 ***********************************************************/
RESM_ERR_t RESM_SV_Open(const RESM_RSV_t* p_prim, uint32_t* p_ssnld);
RESM_ERR_t RESM_SV_Close(uint32_t ssnld);
RESM_ERR_t RESM_SV_ChkSsnId(uint32_t ssnld);

#endif  // RESOURCE_MANAGER_SERVER_INCLUDE_RESM_INTERNAL_H_
