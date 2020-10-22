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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_PowerService
/// \brief    Handles SSM message for Wakeup State, Crank State, and Voltage
///           State.
///
///////////////////////////////////////////////////////////////////////////////

#include "ss_power.h"
#include <stdio.h>
#include <system_service/ss_power_service_notifications.h>
#include <system_service/ss_power_service.h>
#include <system_service/ss_power_service_notifications_local.h>
#include <system_service/ss_system_manager_protocol.h>
#include <system_service/ss_sm_client_if.h>
#include <system_service/ss_templates.h>
#include <system_service/ss_services.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <map>

#include "ss_power_powerservicelog.h"


EFrameworkunifiedStatus Power::OnPowerRequestMsg(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  // ToDo Jay 2012 December 05 Would like to provide better general overall
  // solution then this local hack - but, for the time being...
  std::map<EPWR_USER_MODE_TYPE, EPWR_POWER_STATE_TYPE> UserModeToPowerTypeMap;
  UserModeToPowerTypeMap[epsumON] = epswsPWRON;
  UserModeToPowerTypeMap[epsumOFF] = epswsPWROFF;
  // End of ToDo Jay 2012 December 05 Would like to provide better general

  Pwr_ServiceSetInterface tServiceIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    BOOL b_isValid = FALSE;
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "StartupReason is '%s'",
           GetStr(tServiceIf.data.powerRequestMsg.startupReason).c_str());

    switch (tServiceIf.data.powerRequestMsg.startupReason) {
      case epswfINVALID:
      case epswfTESTACC:
      case epswfUSER_DATA_RESET:
        // Not startup conditions; don't log
        b_isValid = TRUE;
        break;

      case epswfON_KEY:
        LOG_POWERSERVICELOG_EVT(epswfON_KEY, Ei_StartUp_HK_On, epseisuedHK_ON_Key);
        b_isValid = TRUE;
        break;

      case epswfIGN_ACC:
        LOG_POWERSERVICELOG_EVT(epswfIGN_ACC, Ei_StartUp_Cansignal, epseisuedIGNITION);
        LOG_POWERSERVICELOG_CNT(epswfIGN_ACC, CT_Startup_CanSignal);
        b_isValid = TRUE;
        break;

      case epswfDR_OPEN_CLOSE:
        LOG_POWERSERVICELOG_EVT(epswfDR_OPEN_CLOSE, Ei_StartUp_Cansignal,
                       epseisuedDOOR_MODULES);
        LOG_POWERSERVICELOG_CNT(epswfDR_OPEN_CLOSE, CT_Startup_CanSignal);
        b_isValid = TRUE;
        break;

      case epswfDX_ACTIVATION:
        LOG_POWERSERVICELOG_EVT(epswfDX_ACTIVATION, Ei_StartUp_DiagMsg, epseisuedHU);
        LOG_POWERSERVICELOG_CNT(epswfDX_ACTIVATION, CT_Startup_DiagMsg);
        b_isValid = TRUE;
        break;

      case epswfPASS_ACTIVATION:
        LOG_POWERSERVICELOG_EVT(epswfPASS_ACTIVATION, Ei_StartUp_Cansignal,
                       epseisuedPASS);
        LOG_POWERSERVICELOG_CNT(epswfPASS_ACTIVATION, CT_Startup_CanSignal);
        b_isValid = TRUE;
        break;

      case epswfSPVACTIVATION:
        LOG_POWERSERVICELOG_EVT(epswfSPVACTIVATION, Ei_StartUp_Cansignal, epseisuedSNA);
        LOG_POWERSERVICELOG_CNT(epswfSPVACTIVATION, CT_Startup_CanSignal);
        b_isValid = TRUE;
        break;

        // default:  Don't code a 'default' here - let the compiler
        // issue a warning ( set via -Wall or -Wswitch ) when the set of
        // enumerations changes - then the maintainer will
        // automagically know to update this switch statement.
    }  // End switch(tServiceIf.data.powerRequestMsg.startupReason)

    if (FALSE == b_isValid) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_START 11:unexpected branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR, __FUNCTION__,
          " Error: Unknown 'powerRequestMsg.startupReason' value: " "0x%X / %d",
          tServiceIf.data.powerRequestMsg.startupReason,
          tServiceIf.data.powerRequestMsg.startupReason);
      // LCOV_EXCL_STOP 11:unexpected branch
    } else {
      m_WakeUpData.powerupType = UserModeToPowerTypeMap[tServiceIf.data
          .powerRequestMsg.userMode];
      m_WakeUpData.up.factor = tServiceIf.data.powerRequestMsg.startupReason;
      m_WakeUpData.up.level = epswlFULLRUN;
      m_WakeUpData.up.userModeChangeReason = tServiceIf.data.powerRequestMsg.userModeChangeReason;

      l_eStatus = FrameworkunifiedSendMsg(GetSystemManagerSessionHandle(),
                             SS_SM_POWER_REQUEST_MSG, sizeof(tServiceIf),
                             &tServiceIf);
      char l_cBuf[200];
      snprintf(
          l_cBuf, sizeof(l_cBuf),
          "FrameworkunifiedSendMsg( SS_SM_POWER_REQUEST_MSG, %s, %s)",
          GetStr(tServiceIf.data.powerRequestMsg.userMode).c_str(),
          GetStr(tServiceIf.data.powerRequestMsg.userModeChangeReason).c_str());
      LOG_STATUS(l_eStatus, l_cBuf);
    }
  }  // End else a good ReadMsg()

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnpowerRequestMsg( HANDLE h_app )

EFrameworkunifiedStatus Power::OnShutdownRequestMsg(HANDLE h_app) {
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  Pwr_ServiceSetInterface tServiceIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    BOOL b_isValid = FALSE;
    EPWR_SHUTDOWN_TRIGGER_TYPE l_shutdownTrigger = tServiceIf.data
        .shutdownRequestMsg.shutdownTrigger;
    switch (l_shutdownTrigger) {
      case epssdmsdtINVALID:
      case epssdmsdtTESTACC_OFF:
        // Not shutdown conditions; don't log
        b_isValid = TRUE;
        break;

      case epssdmsdtON_KEY:
        LOG_POWERSERVICELOG_EVT(epssdmsdtON_KEY, Ei_shutdown_HkOFF, 0x00);
        b_isValid = TRUE;
        break;

      case epssdmsdtIGN_LOCK:
        LOG_POWERSERVICELOG_EVT(epssdmsdtIGN_LOCK, Ei_shutdown_CANsignal,
                       epseisdcsIGN_STATE);
        LOG_POWERSERVICELOG_CNT(epssdmsdtIGN_LOCK, CT_Shutdown_CanSignal);
        b_isValid = TRUE;
        break;

      case epssdmsdtPWR_SAVE:  // test from peripheralservice/power_supply_manager_shadow
        LOG_POWERSERVICELOG_CNT(epssdmsdtPWR_SAVE, CT_Shutdown_Timeout);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtTMP_STARTUP:
        LOG_POWERSERVICELOG_CNT(epssdmsdtTMP_STARTUP, CT_Shutdown_Timeout);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtDIAG_DEACTIVATION:
        LOG_POWERSERVICELOG_CNT(epssdmsdtDIAG_DEACTIVATION, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtABNORMAL_VOLTAGE:
        LOG_POWERSERVICELOG_EVT(epssdmsdtABNORMAL_VOLTAGE, Ei_shutdown_Voltage,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                       epseisdnvoltUNKNOWN);
        LOG_POWERSERVICELOG_CNT(epssdmsdtABNORMAL_VOLTAGE, CT_Shutdown_Voltage);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtABNORMAL_TEMP:
        LOG_POWERSERVICELOG_EVT(epssdmsdtABNORMAL_TEMP, Ei_shutdown_Temperature, 0x00);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        LOG_POWERSERVICELOG_CNT(epssdmsdtABNORMAL_TEMP, CT_Shutdown_Temperature);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtBATTERYCUTOFF:
        LOG_POWERSERVICELOG_EVT(epssdmsdtBATTERYCUTOFF, Ei_shutdown_Voltage,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                       epseisdnvoltBATTERY_CUT_OFF);
        LOG_POWERSERVICELOG_CNT(epssdmsdtBATTERYCUTOFF, CT_Shutdown_Voltage);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtLIMPHOME:
        LOG_POWERSERVICELOG_EVT(epssdmsdtLIMPHOME, Ei_shutdown_Voltage,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                       epseisdnvoltLIMP_HOME);
        LOG_POWERSERVICELOG_CNT(epssdmsdtLIMPHOME, CT_Shutdown_Voltage);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtHU_CAN_ERROR:
        LOG_POWERSERVICELOG_EVT(epssdmsdtHU_CAN_ERROR, Ei_shutdown_NetworkError,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                       epseisdneHU_CAN);
        LOG_POWERSERVICELOG_CNT(epssdmsdtHU_CAN_ERROR, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtBODY_CAN_ERROR:
        LOG_POWERSERVICELOG_EVT(epssdmsdtBODY_CAN_ERROR, Ei_shutdown_NetworkError,  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
                       epseisdneHMI_CAN);
        LOG_POWERSERVICELOG_CNT(epssdmsdtBODY_CAN_ERROR, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtTRANSPORT_MODE:
        LOG_POWERSERVICELOG_CNT(epssdmsdtTRANSPORT_MODE, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtPRODUCTION_MODE:
        LOG_POWERSERVICELOG_CNT(epssdmsdtPRODUCTION_MODE, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtIGN_OFF:
        LOG_POWERSERVICELOG_CNT(epssdmsdtIGN_OFF, CT_Shutdown_CanSignal);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtGENERIC_ERROR_RESET:
        LOG_POWERSERVICELOG_CNT(epssdmsdtGENERIC_ERROR_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtFATAL_ERROR_RESET:
        LOG_POWERSERVICELOG_CNT(epssdmsdtFATAL_ERROR_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtUSER_DATA_RESET:
        LOG_POWERSERVICELOG_CNT(epssdmsdtUSER_DATA_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtFACTORY_DATA_RESET:
        LOG_POWERSERVICELOG_CNT(epssdmsdtFACTORY_DATA_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtFAST_SLEEP_MODE:
        LOG_POWERSERVICELOG_CNT(epssdmsdtFAST_SLEEP_MODE, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtNORMAL_RESET:
        LOG_POWERSERVICELOG_CNT(epssdmsdtNORMAL_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;

      case epssdmsdtPROGUPDATE_RESET:  // this is dead code
        LOG_POWERSERVICELOG_CNT(epssdmsdtPROGUPDATE_RESET, CT_Shutdown_Error);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
        b_isValid = TRUE;
        break;
        // default:  Don't code a 'default' here - let the compiler
        // issue a warning ( set via -Wall or -Wswitch ) when the set of
        // enumerations changes - then the maintainer will
        // automagically know to update this switch statement.
    }  // End switch(l_shutdownTrigger)
    if (FALSE == b_isValid) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_START 11:unexpected branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          " Error: Unknown 'shutdownRequestMsg.shutdownTrigger' " "value: 0x%X/%d",
          l_shutdownTrigger, l_shutdownTrigger);
      // LCOV_EXCL_STOP 11:unexpected branch
    } else {
      l_eStatus = SendShutdownToSystemManager(&tServiceIf);
      LOG_STATUS(l_eStatus, "SendShutdownToSystemManager()");
      if (l_eStatus == eFrameworkunifiedStatusOK) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
        SetPowerServiceState(SS_PS_SHUTDOWN_INITIATED);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnShutdownRequestMsg( HANDLE h_app )

//*****************************************************************************
// Shutdown Condition Notification Protocol function
//
EFrameworkunifiedStatus Power::OnPublishShutdownPopupRequest(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;
  Pwr_ServiceSetInterface tServiceIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    BOOL b_isValid = FALSE;
    EPWR_SHUTDOWN_POPUP_TYPE l_eShutdownPopup = tServiceIf.data.shutdownPopup
        .shutdownPopupEvent;
    switch (l_eShutdownPopup) {
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
        l_eStatus = FrameworkunifiedNPPublishNotification(h_app
            , szNTFY_ShutdownPopup
            , &l_eShutdownPopup
            , sizeof(EPWR_SHUTDOWN_POPUP_TYPE));
        if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
          // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: FrameworkunifiedNPPublishNotification(%s, %d) errored: %d/'%s'",
                 szNTFY_ShutdownPopup, l_eShutdownPopup, l_eStatus,
                 GetStr(l_eStatus).c_str());
          // LCOV_EXCL_STOP 4: NSFW error case.
        }

        SS_PWR_LOG_HIST("szNTFY_ShutdownPopup()", m_PubCmdHist, m_PubHistIter,
                        "", l_eStatus);

        b_isValid = TRUE;
        break;

        // default:  Don't code a 'default' here - let the compiler
        // issue a warning ( set via -Wall or -Wswitch ) when the set of
        // enumerations changes - then the maintainer will
        // automagically know to update this switch statement.
    }  // End switch

    if (FALSE == b_isValid) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_START 11:unexpected branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error: Unknown 'l_eShutdownPopup' value: 0x%X / %d",
             l_eShutdownPopup, l_eShutdownPopup);
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      // LCOV_EXCL_STOP 11:unexpected branch
    } else {
      EFrameworkunifiedStatus l_responseStatus = l_eStatus;  // Save the current status as-is to
                                                // send in the message response to the
                                                // requester.
      l_eStatus = FrameworkunifiedSendResponse(h_app,
                                  SS_POWER_PUBLISH_SHUTDOWN_CONDITION_RESP,
                                  sizeof(EFrameworkunifiedStatus),
                                  (PVOID) &l_responseStatus);

      char l_cBuf[200];
      snprintf(l_cBuf, sizeof(l_cBuf),
               "FrameworkunifiedSendResponse(%s, %s, status: %d/'%s')", FrameworkunifiedGetMsgSrc(h_app),
               "SS_POWER_PUBLISH_SHUTDOWN_CONDITION_RESP", l_responseStatus,
               GetStr(l_responseStatus).c_str());
      LOG_STATUS(l_eStatus, l_cBuf);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnPublishShutdownPopupRequest( HANDLE h_app )
//
// End of Shutdown Condition Notification Protocol function
//*****************************************************************************

//*****************************************************************************
// Power Popup Notification Protocol function
//
EFrameworkunifiedStatus Power::OnPublishPowerPopupRequest(HANDLE h_app) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  Pwr_ServiceSetInterface tServiceIf;

  // ReadMsg():                                                        *
  //     Check h_app ptr, msg size, msg reception, read msg if all ok.  *
  //     Report any errors found.                                      *
  //                                                                   *
  if (eFrameworkunifiedStatusOK != (l_eStatus = ReadMsg<Pwr_ServiceSetInterface>(h_app, tServiceIf))) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.  // NOLINT[whitespace/line_length]
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    LOG_ERROR("ReadMsg()");  // LCOV_EXCL_LINE 4: NSFW error case.
  } else {
    BOOL b_isValid = FALSE;
    EPWR_POWER_POPUP_TYPE l_ePowerPopup = tServiceIf.data.powerPopup
        .powerPopupEvent;
    switch (l_ePowerPopup) {
      case epssppNormal:
      case epssppCritical:
      case epssppAppCritical:
      case epssppAllClr:
        l_eStatus = FrameworkunifiedNPPublishNotification(h_app
            , szNTFY_PowerPopup
            , &l_ePowerPopup
            , sizeof(EPWR_POWER_POPUP_TYPE));
        if (eFrameworkunifiedStatusOK != l_eStatus) {  // LCOV_EXCL_BR_LINE 4: NSFW error case.
            // LCOV_EXCL_START 4: NSFW error case.
          AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
                 " Error: FrameworkunifiedNPPublishNotification(%s, %d) errored: %d/'%s'",
                 szNTFY_PowerPopup, l_ePowerPopup, l_eStatus,
                 GetStr(l_eStatus).c_str());
            // LCOV_EXCL_STOP 4: NSFW error case.
        }

        SS_PWR_LOG_HIST("OnPublishPowerPopupRequest()", m_PubCmdHist,
                        m_PubHistIter, "", l_eStatus);

        b_isValid = TRUE;
        break;

        // default:  Don't code a 'default' here - let the compiler
        // issue a warning ( set via -Wall or -Wswitch ) when the set of
        // enumerations changes - then the maintainer will
        // automagically know to update this switch statement.
    }  // End switch

    if (FALSE == b_isValid) {  // LCOV_EXCL_BR_LINE 11:unexpected branch
      // LCOV_EXCL_START 11:unexpected branch
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             " Error: Unknown 'l_ePowerPopup' value: 0x%X/%d", l_ePowerPopup,
             l_ePowerPopup);
      l_eStatus = eFrameworkunifiedStatusInvldParam;
      // LCOV_EXCL_STOP 11:unexpected branch
    } else {
      EFrameworkunifiedStatus l_responseStatus = l_eStatus;  // Save the current status as-is to
                                                // send in the message response to the
                                                // requester.
      l_eStatus = FrameworkunifiedSendResponse(h_app, SS_POWER_PUBLISH_POWER_POPUP_RESP,
                                  sizeof(EFrameworkunifiedStatus),
                                  (PVOID) &l_responseStatus);
      char l_cBuf[200];
      snprintf(l_cBuf, sizeof(l_cBuf),
               "FrameworkunifiedSendResponse(%s, %s, status: %d/'%s')", FrameworkunifiedGetMsgSrc(h_app),
               "SS_POWER_PUBLISH_POWER_POPUP_RESP", l_responseStatus,
               GetStr(l_responseStatus).c_str());
      LOG_STATUS(l_eStatus, l_cBuf);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // End of EFrameworkunifiedStatus Power::OnPublishPowerPopupRequest( HANDLE h_app )
//
// End of Power Popup Notification Protocol function
//*****************************************************************************

// EOF of /SS_PowerService/src/ss_supervisor_handlers.cpp
