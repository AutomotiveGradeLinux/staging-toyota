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
/// \ingroup  tag_SystemServices_Logger
/// \brief    This file supports the _CWORD77_ logger service.
///
///////////////////////////////////////////////////////////////////////////////
#include "system_service/ss_logger_service_ifc.h"
#include <string.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_types.h>
#include <string>
#include "system_service/ss_logger_service_protocol.h"
#include "system_service/ss_logger_service_notifications.h"
#include "system_service/ss_logger_service.h"
#include "system_service/ss_logger_service_local.h"
#include "system_service/ss_services.h"
#include "ss_logger_service_if_interfaceunifiedlog.h"

LoggerServiceIf::LoggerServiceIf()
    : m_hApp(NULL),
      m_hService(NULL),
      m_hSession(NULL) {
}

LoggerServiceIf::~LoggerServiceIf() {
  CloseSessionRequest();  // if the session is still connected

  m_hService = NULL;
  m_hSession = NULL;
  m_hApp = NULL;
}  // LCOV_EXCL_BR_LINE 10:Because destructor

EFrameworkunifiedStatus LoggerServiceIf::NotifyOnLoggerServiceAvailability(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    // Subscriptions
    FrameworkunifiedNotificationCallbackHandler g_aryLogger_Notif_Cbs[] = {
    // Notifications name,              Call back function
        { NTFY_SS_LoggerService_Availability, f_pCallBackFn }, };

    if (0 != strcmp(SERVICE_LOGGER, FrameworkunifiedGetAppName(m_hApp))) {
      // Subscribe and attach call backs to notifications
      if (eFrameworkunifiedStatusOK
          != (eStatus = FrameworkunifiedSubscribeNotificationsWithCallback(
              m_hApp, g_aryLogger_Notif_Cbs, _countof(g_aryLogger_Notif_Cbs)))) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
               "FrameworkunifiedAttachNotificationCallbacksToDispatcher Failed Status:0x%x ",
               eStatus);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::NotifyOnOpenSessionAck(CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
        SERVICE_LOGGER,
                                                    PROTOCOL_OPEN_SESSION_ACK,
                                                    f_pCallBackFn))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ", eStatus);
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::NotifyOnCloseSessionAck(CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hApp && NULL != f_pCallBackFn) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
        SERVICE_LOGGER,
                                                    PROTOCOL_CLOSE_SESSION_ACK,
                                                    f_pCallBackFn))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ", eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

BOOL LoggerServiceIf::Initialize(HANDLE hApp) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  BOOL l_eStatus = TRUE;

  if (NULL != hApp) {
    m_hApp = hApp;
  } else {
    l_eStatus = FALSE;  //  eFrameworkunifiedStatusInvldHandle;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::OpenSessionRequest() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  if (NULL != m_hApp) {
    // if the session is there then we need to close it first!
    if (NULL != m_hService) {
      CloseSessionRequest();
      m_hService = NULL;
    }
    BOOL l_bServiceAvailable = FrameworkunifiedIsServiceAvailable(m_hApp);
    if (FALSE == l_bServiceAvailable) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__, "Service is unavailable");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      // Now open the service.
      if (l_bServiceAvailable
          && (NULL != (m_hService = OpenService()))) {  // LCOV_EXCL_BR_LINE 8:Because bServiceAvailable is always TRUE
        // [DM: TODO] Check if we need to open a session with some data sent to Server.
        UI_32 l_pTestData = 1;
        if (eFrameworkunifiedStatusOK
            != (l_eStatus = FrameworkunifiedOpenSessionWithData(m_hService,
                                                   (PVOID) &l_pTestData,
                                                   sizeof(UI_32)))) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
                 "FrameworkunifiedOpenSessionWithData Failed, errval :%d", l_eStatus);
        }
      } else {
        l_eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
               "FrameworkunifiedOpenService Failed, errval :%d", l_eStatus);
      }
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

HANDLE LoggerServiceIf::OpenService() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  HANDLE l_hService = NULL;

  if (NULL == (l_hService = FrameworkunifiedOpenService(m_hApp, SERVICE_LOGGER))) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
           "FrameworkunifiedOpenService : Failed to open Logger Service");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_hService;
}

EFrameworkunifiedStatus LoggerServiceIf::DecodeOpenSessionResponse() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  std::string l_strSessionName;
  if (NULL != m_hService
      && NULL != m_hApp) {  // LCOV_EXCL_BR_LINE 8:Because there is no case where m_hApp is NULL and m_hService is not NULL
    if (NULL == (m_hSession = FrameworkunifiedGetOpenSessionHandle(m_hApp))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __PRETTY_FUNCTION__,
             "Error in extracting OpenSessionAck response.");
      l_eStatus = eFrameworkunifiedStatusFail;
    } else {
      l_eStatus = eFrameworkunifiedStatusOK;
      l_strSessionName = FrameworkunifiedGetSessionName(m_hSession);
      if (l_strSessionName.empty()) {
        l_eStatus = eFrameworkunifiedStatusFail;
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SessionName is empty");
      } else {
        if (eFrameworkunifiedStatusOK
            != (FrameworkunifiedSetSessionHandle(m_hApp, l_strSessionName.c_str(), m_hSession))) {
          l_eStatus = eFrameworkunifiedStatusFail;
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Error: FrameworkunifiedSetSessionHandle() failed");
        }
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::CloseSessionRequest() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK != (l_eStatus = FrameworkunifiedCloseSession(m_hService, m_hSession))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedCloseSession Failed");
    }
    m_hSession = NULL;  // clear our session handle
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::RegisterForLoggerEvent(
    SS_LoggerServerEvents f_eLoggerEvent, CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hSession && NULL != f_pCallBackFn) {
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedAttachCallbackToDispatcher(m_hApp,
        SERVICE_LOGGER,
                                                    f_eLoggerEvent,
                                                    f_pCallBackFn, m_hSession))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "FrameworkunifiedAttachCallbackToDispatcher Failed Status:0x%x ", eStatus);
    }

    // client registers for the event
    if (eFrameworkunifiedStatusOK
        != (eStatus = FrameworkunifiedRegisterEvent(m_hSession, f_eLoggerEvent))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedRegisterEvents Failed Status:0x%x",
             eStatus);
    }
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "FrameworkunifiedRegisterEvents Status:0x%x", eStatus);
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::UnRegisterForLoggerEvent(
    SS_LoggerServerEvents f_eLoggerEvent) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hSession) {
    eStatus = eFrameworkunifiedStatusOK;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// SendDiagStat
/// API to send CAN Diagnostic status data(it has mileage info) to Logger Service
/// (Command: SS_LOGGER_MILEAGE_DATA)
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////

EFrameworkunifiedStatus LoggerServiceIf::SendDiagStat(STLOGGER_CANDIAGSTAT *pCanDiagStat_t) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  STLOGGER_CANDIAGSTAT l_candiagstat;

  memcpy(&l_candiagstat, pCanDiagStat_t, sizeof(STLOGGER_CANDIAGSTAT));

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Before Valid Session");
  if (NULL != m_hService && NULL != m_hSession) {
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Entered Valid Session");
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_MILEAGE_DATA,
                                   sizeof(STLOGGER_CANDIAGSTAT), &l_candiagstat))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
    FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "Msg Send Successfully");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_CANGetCurrentDateAndTime
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_CANGetCurrentDateAndTime(
    STCanCurrentDateTime stDateAndTime) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SET_DATETIME,
                                   sizeof(stDateAndTime), &stDateAndTime))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_CANSetVIN
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_CANSetVIN(STVIN_NUMBER& stVinNumber) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  stVinNumber.VINstr[stVinNumber.VIN_LEN - 1] = '\0';

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SET_VIN,
                                   sizeof(STVIN_NUMBER), &stVinNumber))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg() Failed with error: %X",
             l_eStatus);
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Service or Session Handle NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// LoggerIf_SetLoggerParams
/// API is to inform Logger Service about the device selected to store the logs
/// and about Enable/Disable Logging.
/// (Command: SS_LOGGER_SET_PARAMS)
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_SetLoggerParams(ELOGGER_STAT eLoggerStatus,
                                                     EDEV_TYPE eDevType) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  STLoggerSetParams l_stLoggerParams;

  l_stLoggerParams.Logger_State = eLoggerStatus;
  l_stLoggerParams.Device_Type = eDevType;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SET_PARAMS,
                                   sizeof(STLoggerSetParams), &l_stLoggerParams))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// LoggerIf_UDPLogging
/// API is to inform Logger Service about UDP logging
/// and about Enable/Disable Logging.
/// (Command: SS_LOGGER_UDP_LOGGING)
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_UDPLogging(ELOGGER_STAT eLoggerStatus) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_UDP_LOGGING,
                                   sizeof(ELOGGER_STAT), &eLoggerStatus))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_ScreenCaptureEventACK(
    STScreenCaptureEvt stScreenCaptureInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SCREENCAPTURE_EVT_ACK,
                                   sizeof(stScreenCaptureInfo),
                                   &stScreenCaptureInfo))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, " Screen shot ACK sent successfully.");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error. m_hSession is NULL. Screen shot capture ACK not sent.");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_EventLoggingCommonInfo
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_EventLoggingCommonInfo(
    STEventLoggerCommonInfo stEventCommonInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_EVENT_COMMONINFO,
                                   sizeof(stEventCommonInfo),
                                   &stEventCommonInfo))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_EventLoggingEventInfo
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_EventLoggingEventInfo(
    STEventLoggerEventInfo stEventInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER__CWORD56__EVENT_INFO,
                                   sizeof(stEventInfo), &stEventInfo))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_EventLoggingResetInfo
/// (Command: SS_LOGGER__CWORD56__RESET_INFO)
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_EventLoggingResetInfo(
    STEventLoggerResetInfo stResetInfo) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER__CWORD56__RESET_INFO,
                                   sizeof(stResetInfo), &stResetInfo))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_EventLoggingResetInfo
/// (Command: SS_LOGGER_DIAGDTC_ACTIVE)
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_PersistEventLogOnActiveDTC(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_DIAGDTC_ACTIVE, 0x00,
                                   NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_EventLoggingResetInfo
/// (Command: SS_LOGGER_SHUTDOWN_COMPLETE)
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_Shutdown_Complete(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != m_hService && NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SHUTDOWN_COMPLETE,
                                   0x00, NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :StoreEventLogsToUSB
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::StoreEventLogsToUSB(
    EEvtLogUSBDevNumber eUSBDevNumber) {

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Copy the Event logs in to USB command received:%X", eUSBDevNumber);

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGERCOPYEVENTUSB,
                                   sizeof(EEvtLogUSBDevNumber), &eUSBDevNumber))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :StoreEmergencyErrorLogsToUSB
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::StoreEmergencyErrorLogsToUSB(
    EDevNumber eDevNumber) {

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(
      ZONE_INFO, __FUNCTION__,
      "Copy the Emergency Error logs in to External Device command received:%X",
      eDevNumber);

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGERCOPYEMERGENCYLOGS,
                                   sizeof(EDevNumber), &eDevNumber))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :ClearEventLogs
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::ClearEventLogs(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Request for clearing the event logs");

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGERCLEAREVENT, 0, NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :ReadStatisticalCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::ReadStatisticalCounter(
    EStatCounterGroupID eCounterGroup) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Request to read the statistical counter for group ID:%X",
         eCounterGroup);

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_READ_STATL_COUNTER,
                                   sizeof(EStatCounterGroupID), &eCounterGroup))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :ReadStatisticalCounter
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::ResetStatisticalCounter(
    EStatCounterGroupID eCounterGroup) {

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "Request to reset the statistical counter for group ID:%X",
         eCounterGroup);

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_RESET_STATL_COUNTER,
                                   sizeof(EStatCounterGroupID), &eCounterGroup))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function LoggerServiceIf::StartDTCLoggingToEmmc
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::StartDTCLoggingToEmmc(UI_32 f_dtc) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Request to start logging for DTC:0x%4X",
         f_dtc);
  l_eStatus = this->SendMessage(eSSLoggerCANProtocolIDDTCTrigger,
                                (UI_32) sizeof(f_dtc), &f_dtc);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function LoggerServiceIf::StartCANLogging
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::StartCANLogging(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "Request to start CAN logging.");
  l_eStatus = this->SendMessage(eSSLoggerCANProtocolIDCANTrigger, 0, NULL);
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function LoggerServiceIf::RegisterLoggingStartNotification
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::RegisterLoggingStartNotification(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == this->m_hApp) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "(NULL == this->m_hApp)");
  } else if (NULL == f_pCallBackFn) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "(NULL == f_pCallBackFn)");
  } else {
    l_eStatus = FrameworkunifiedSubscribeToSessionEventWithCallback(this->m_hApp,
                                                       eSSLoggerCANEventStart,
                                                       f_pCallBackFn,
                                                       this->m_hSession);
    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "FrameworkunifiedSubscribeToSessionEventWithCallback(eSSLoggerCANEventStart) returned %X",
          l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function LoggerServiceIf::RegisterLoggingStartNotification
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::RegisterLoggingFinishNotification(
    CbFuncPtr f_pCallBackFn) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus;

  if (NULL == this->m_hApp) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "(NULL == this->m_hApp)");
  } else if (NULL == f_pCallBackFn) {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "(NULL == f_pCallBackFn)");
  } else {
    l_eStatus = FrameworkunifiedSubscribeToSessionEventWithCallback(
        this->m_hApp, eSSLoggerCANEventFinished, f_pCallBackFn,
        this->m_hSession);

    if (eFrameworkunifiedStatusOK != l_eStatus) {
      FRAMEWORKUNIFIEDLOG(
          ZONE_ERR,
          __FUNCTION__,
          "FrameworkunifiedSubscribeToSessionEventWithCallback(eSSLoggerCANEventFinished) returned %X",
          l_eStatus);
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

///////////////////////////////////////////////////////////////////////
/// Function :SendMessage
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::SendMessage(UI_32 f_cmdID, UI_32 f_size,
                                        void* f_pData) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusInvldHandle;

  if ((NULL != m_hSession)
      && ((f_pData != NULL) || (f_size == 0))) {  // LCOV_EXCL_BR_LINE 8:Because there is no case where f_pData is NULL and f_size is not 0
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, f_cmdID, f_size, f_pData))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed");
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_SetDiagID
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_SetDiagID(UI_16 f_u16DiagID) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_SET_DIAGID,
                                   sizeof(UI_16), &f_u16DiagID))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed %d", l_eStatus);
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}

///////////////////////////////////////////////////////////////////////
/// Function :LoggerIf_UploadEventLog
///////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus LoggerServiceIf::LoggerIf_UploadEventLog() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;

  if (NULL != m_hSession) {
    if (eFrameworkunifiedStatusOK
        != (l_eStatus = FrameworkunifiedSendMsg(m_hSession, SS_LOGGER_UPLOAD_EVENTLOG, 0x00,
                                   NULL))) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "FrameworkunifiedSendMsg Failed %d", l_eStatus);
    }
  } else {
    l_eStatus = eFrameworkunifiedStatusNullPointer;
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "Session failed");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return l_eStatus;
}  // LCOV_EXCL_BR_LINE 10:Because the last line
