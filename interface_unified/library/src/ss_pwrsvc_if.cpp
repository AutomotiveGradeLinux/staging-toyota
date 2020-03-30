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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_PowerServiceIf
/// \brief    This file supports the Power Service module interface.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_power_service_if.h"
#include <string.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include "system_service/ss_power_service.h"
#include "system_service/ss_power_service_protocol.h"
#include "system_service/ss_power_service_notifications.h"
#include "system_service/ss_system_if.h"
#include "system_service/ss_services.h"
#include "system_service/ss_string_maps.h"
#include "system_service/ss_templates.h"
#include "ss_power_if_interfaceunifiedlog.h"

HANDLE OpenPowerService(HANDLE f_hApp) {
  EFrameworkunifiedStatus l_eStatus;
  HANDLE l_hService = NULL;
  if (NULL == f_hApp) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_App");
  } else if (NULL == (l_hService = FrameworkunifiedOpenService(f_hApp, SERVICE_POWER))) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    LOG_ERROR("FrameworkunifiedOpenService(f_hApp, SERVICE_POWER)");
  }
  return l_hService;
}

EFrameworkunifiedStatus ClosePowerService(HANDLE f_hApp, HANDLE f_hService) {
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hApp) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_App");
  } else if (NULL == f_hService) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hService");
  } else {
    l_eStatus = FrameworkunifiedCloseService(f_hApp, f_hService);
    LOG_STATUS(l_eStatus, "FrameworkunifiedCloseService(f_hApp, f_hService)");
    f_hService = NULL;
  }
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Session Related Requests
/// Any client can also use FrameworkunifiedOpenSession and FrameworkunifiedOpenSessionWithData.
/// In that case, the PVOID data passed as a parameter should be MM_OpenSessionRequestIf
/// to open a session with the Power Service.
/// The client can also use the below API for Opening a session with Power Service.
///////////////////////////////////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus PwrServiceOpenSessionRequest(HANDLE f_hService,
                                        EPWR_SESSION_TYPE f_eSessionType) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == f_hService) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hService");
  } else if (epsstUNKNOWN == f_eSessionType) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("epsstUNKNOWN == f_eSessionType");
  } else {
    l_eStatus = FrameworkunifiedOpenSessionWithData(f_hService, &f_eSessionType,
                                       sizeof(f_eSessionType));
    LOG_STATUS(l_eStatus, "FrameworkunifiedOpenSessionWithData()");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceCloseSessionRequest(HANDLE f_hService, HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == f_hService) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hService");
  } else if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    CALL_AND_LOG_STATUS(FrameworkunifiedCloseSession(f_hService, f_hSession));
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceDecodeOpenSessionResponse(HANDLE f_hApp,
                                               HANDLE& f_hSession) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL == f_hApp) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_App");
  } else {
    if (NULL == (f_hSession = FrameworkunifiedGetOpenSessionHandle(f_hApp))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error: FrameworkunifiedGetOpenSessionHandle('%s') returned NULL",
             FrameworkunifiedGetAppName(f_hApp));
      l_eStatus = eFrameworkunifiedStatusNullPointer;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceSetVoltageState(HANDLE f_hSession,
                                     EPWR_VOLTAGE_STATE_TYPE f_eVoltage_state) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else if (epsvsINVALID == f_eVoltage_state) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("epsvsINVALID == f_eVoltage_state");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    l_tServiceSetIf.data.voltage.state = f_eVoltage_state;
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_VOLTAGE_STATE,
                           sizeof(l_tServiceSetIf), (PVOID) &l_tServiceSetIf);
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_VOLTAGE_STATE)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceSetCrankState(HANDLE f_hSession,
                                   EPWR_CRANK_STATE_TYPE f_eCrank_state) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else if (epscsINVALID == f_eCrank_state) {
    l_eStatus = eFrameworkunifiedStatusInvldParam;
    LOG_ERROR("epsvsINVALID == f_eCrank_state");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    l_tServiceSetIf.data.crank.state = f_eCrank_state;
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_CRANK_STATE,
                           sizeof(l_tServiceSetIf), (PVOID) &l_tServiceSetIf);
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_CRANK_STATE)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceSendShutdownRequest(
    HANDLE f_hSession, EPWR_SHUTDOWN_REQUEST_MSG_STRUCT &f_eState) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    l_tServiceSetIf.data.shutdownRequestMsg = f_eState;
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_SHUTDOWN_REQUEST_MSG,
                           sizeof(l_tServiceSetIf), &l_tServiceSetIf);

    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_SHUTDOWN_REQUEST_MSG)");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus PwrServiceSendShutdownRequest(

EFrameworkunifiedStatus PwrServiceSendSetShutdownPopupRequest(
    HANDLE f_hSession, EPWR_SHUTDOWN_POPUP_TYPE f_eShutdownPopup) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    switch (f_eShutdownPopup) {
      case epsspPowerSave1:
      case epsspPowerSave2:
      case epsspPowerSave3:
      case epsspPowerSaveClr:
      case epsspLowVoltage1:
      case epsspLowVoltage2:
      case epsspLowVoltage3:
      case epsspLowVoltageClr:
      case epsspBattCouplingSW1:
      case epsspBattCouplingSW2:
      case epsspBattCouplingSW3:
      case epsspBattCouplingSWClr:
      case epsspAbnormalTemp_1st:
      case epsspAbnormalTemp_Clr:
      case epsspLimpHome_1st:
      case epsspLimpHome_2nd:
      case epsspLimpHome_3rd:
      case epsspLimpHome_Clr:
      case epsspProdMd_1st:
      case epsspProdMd_Clr:
      case epsspTransMd_1st:
      case epsspTransMd_Clr:
      case epsspAllClr:
        l_tServiceSetIf.data.shutdownPopup.shutdownPopupEvent =
            f_eShutdownPopup;
        l_eStatus = FrameworkunifiedSendMsg(f_hSession,
                               SS_POWER_PUBLISH_SHUTDOWN_CONDITION_REQ,
                               sizeof(l_tServiceSetIf), &l_tServiceSetIf);
        LOG_STATUS(l_eStatus,
                   "FrameworkunifiedSendMsg(SS_POWER_PUBLISH_SHUTDOWN_CONDITION_REQ)");
        break;

      default:
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               " Error: Unknown 'ShutdownPopup' value: 0x%X/%d",
               f_eShutdownPopup, f_eShutdownPopup);
        l_eStatus = eFrameworkunifiedStatusInvldParam;
        break;
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus PwrServiceSendSetShutdownPopupRequest( HANDLE f_hSession,

EFrameworkunifiedStatus PwrServiceSendStartupConfirmationMsgRequest(
    HANDLE f_hSession, EPWR_SC_MSG_STRUCT f_eState) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    l_tServiceSetIf.data.startupConfirmationMsg = f_eState;
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_FWD_START_CONFIRMATION_MSG_REQ,
                           sizeof(l_tServiceSetIf), &l_tServiceSetIf);

    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_FWD_START_CONFIRMATION_MSG_REQ)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus PwrServiceSendStartupConfirmationMsgRequest ( HANDLE f_hSession,

EFrameworkunifiedStatus PwrServiceSystemModeInfoRequest(HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_SYSTEM_MODE_INFO_REQ, 0, NULL);
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_SYSTEM_MODE_INFO_REQ)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceSendInitCompReport(HANDLE f_hSession) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_INITCOMP_REP, 0, NULL);
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_INITCOMP_REP)");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus PwrServiceSendPowerRequest(
    HANDLE f_hSession, EPWR_POWER_REQUEST_MSG_STRUCT_WITH_UMCR &f_eState) {  // NOLINT (runtime/references)
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    Pwr_ServiceSetInterface l_tServiceSetIf;
    std::memset(&l_tServiceSetIf, 0, sizeof(l_tServiceSetIf));
    l_tServiceSetIf.data.powerRequestMsg = f_eState;
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_POWER_REQUEST_MSG,
                           sizeof(l_tServiceSetIf), (PVOID) &l_tServiceSetIf);

    char l_cBuf[200] = { 0 };
    snprintf(
        l_cBuf,
        sizeof(l_cBuf),
        "FrameworkunifiedSendMsg(SS_POWER_POWER_REQUEST_MSG(%s, %s))",
        GetStr(l_tServiceSetIf.data.powerRequestMsg.userMode).c_str(),
        GetStr(l_tServiceSetIf.data.powerRequestMsg.userModeChangeReason).c_str());
    LOG_STATUS(l_eStatus, l_cBuf);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus PwrServiceSendPowerRequest( HANDLE f_hSession

EFrameworkunifiedStatus PwrServiceSendHeartBeatRequest(HANDLE f_hSession,
                                          EPWR_HB_REQ_MSG_STRUCT *f_eHbReqMsg) {
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  if (NULL == f_hSession) {
    l_eStatus = eFrameworkunifiedStatusInvldHandle;
    LOG_ERROR("NULL == f_hSession");
  } else {
    l_eStatus = FrameworkunifiedSendMsg(f_hSession, SS_POWER_HEARTBEAT_REQ,
                           sizeof(EPWR_HB_REQ_MSG_STRUCT),
                           reinterpret_cast<void *>(f_eHbReqMsg));
    LOG_STATUS(l_eStatus, "FrameworkunifiedSendMsg(SS_POWER_HEARTBEAT_REQ)");
  }
  FRAMEWORKUNIFIEDLOG0(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // LCOV_EXCL_BR_LINE 10:Because the last line
