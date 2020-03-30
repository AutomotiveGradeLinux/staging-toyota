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

#include <other_service/rpc.h>
#include "resm_internal.h"
#include "resmgr_api.h"
#include "resmgr_api_resourcemanagerlog.h"

/**********************************************
 * External variable definitions
 **********************************************/
RPC_ID __thread rpcId;

/*******************************************************************************
 * RPC public API
 *******************************************************************************/
/* Connection */
RESM_ERR_t
RESM_Open(const RESM_RSV_t* p_prim, uint32_t* p_ssnId)
{

  RESM_ERR_t resmRet;
  EV_ERR evRet;

  // Argument check
  if( p_ssnId == NULL ) {
    return RESM_E_PAR;
  }

  // RPC resources
  if( RPC_OK != RPC_START_CLIENT(&rpcId) ) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "[RESM_ERR]ResMgr Open: RPC_START Error"); // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    return RESM_E_NG;
  }


  // Session connection
  resmRet = RESM_SV_Open(p_prim, p_ssnId);
  if( resmRet != RESM_E_OK ) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "[RESM_ERR]ResMgr Open: Open Session Error. ret[%d]", resmRet); // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    return RESM_E_NG;
  }

  // Create Event Flag
  evRet = EV_create_flag(Resm_Flag_ID_Base + *p_ssnId);
  if( evRet != EV_OK ) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "[RESM_ERR]ResMgr Open: create_flag Error. ret[%d]", evRet);
    return RESM_E_NG;
  }

  return RESM_E_OK;
}

/* Disconnection */
RESM_ERR_t
RESM_Close(uint32_t ssnId)
{

  RESM_ERR_t resmRet;

  // Disconnect session
  resmRet = RESM_SV_Close(ssnId);
  if( resmRet != RESM_E_OK ) {
    return resmRet;
  }

  // Destroy RPC Resources
  RPC_end(rpcId);

  return RESM_E_OK;
}

/* Get event FD */
RESM_ERR_t
RESM_GetEventFd(uint32_t ssnId, int32_t* p_fd)
{

  RESM_ERR_t resmRet;

  // Argument check
  if( p_fd == NULL ) {
    return RESM_E_PAR;
  }
  // Session ID check
  resmRet = RESM_SV_ChkSsnId(ssnId);
  if( resmRet != RESM_E_OK ) {
    return RESM_E_PAR;
  }

  if( EV_OK  != EV_get_flag_fd(Resm_Flag_ID_Base + ssnId, p_fd) ) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "[RESM_ERR]ResMgr GetEventFd Error");
    return RESM_E_NG;
  }

  return RESM_E_OK;
}

/* Get event */
RESM_ERR_t
RESM_GetEvent(uint32_t ssnId, RESM_EV_t* p_evFlag)
{

  RESM_ERR_t resmRet;
  EV_Flag flag;

  // Argument check
  if( p_evFlag == NULL ) {
    return RESM_E_PAR;
  }
  // Session ID Check
  resmRet = RESM_SV_ChkSsnId(ssnId);
  if( resmRet != RESM_E_OK ) {
    return RESM_E_PAR;
  }

  if( EV_OK != EV_get_flag(Resm_Flag_ID_Base + ssnId, &flag)) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "[RESM_ERR]ResMgr GetEvent(get_flag) Error");
    return RESM_E_NG;
  }

  if( flag.bits == 0 ) {
    // No event
    return RESM_E_NG;
  }

  *p_evFlag = flag.bits;

  return RESM_E_OK;
}
