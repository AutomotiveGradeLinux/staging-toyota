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
/// \ingroup  tag_NSFramework
/// \brief    Framework wrapper over the logger interface APIs
///
///
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_rcs_logger_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/ns_logger_if.h>
#include <native_service/nslogutil_cmd_if.h>
#include <native_service/ns_message_center_if.h>
#include <cstdio>
#include "frameworkunified_framework_core.h"
#include "frameworkunified_framework_internal.h"
#include "frameworkunified_framework_utility.h"

FrameworkunifiedProtocolCallbackHandler rcs_logging_callbacks[] = {
  { NSRCS_SET_LOG_SETTINGS_REQ,   NSRcsSetLogSettings },
  { NSRCS_GET_LOG_SETTINGS_REQ,   NSRcsGetLogSettings },
  { NSRCS_SET_LOG_MASK_REQ,       NSRcsSetLogMask },
  { NSRCS_SET_LOG_OUT_OPT_REQ,    NSRcsSetOutputLogOpts },
  { NSRCS_SET_LOG_SEVERITY_REQ,     NSRcsSetLogSeverity },
};


FrameworkunifiedProtocolCallbackHandler logging_callbacks[] = {
  { SET_LOG_MASK,         FrameworkunifiedSetLogMask },
  { GET_LOG_MASK,         FrameworkunifiedGetLogMask },
  { SET_LOG_OUT_OPT,        FrameworkunifiedSetOutputLogOpts },
  { GET_LOG_OUT_OPT,        FrameworkunifiedGetOutputLogOpts },
  // { kDebugDumpRequest,      FrameworkunifiedDebugDumpRequest }, /// < Added to help Debug Issues with in CCR Radio
  // Callback attach with frameworkunifieddebug is moved to SystemServices
  { SET_LOG_SEVERITY,       FrameworkunifiedSetLogSeverity },
  { GET_LOG_SEVERITY,       FrameworkunifiedGetLogSeverity },
};


////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedAttachLoggerCallbacksDispatcher(HANDLE hApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    eStatus = FrameworkunifiedAttachCallbacksToDispatcher(hApp, "nslogutil", logging_callbacks,
                                             static_cast<UI_32>(_countof(logging_callbacks)));
    // Callback attach with of kDebugDumpRequest with frameworkunifieddebug is moved to SystemServices. Keep only nslogutil.
    eStatus = (eStatus != eFrameworkunifiedStatusOK) ? eStatus : FrameworkunifiedAttachCallbackToDispatcher(hApp, "nslogutil", kDebugDumpRequest,
                                                                                  FrameworkunifiedDebugDumpRequest);
    eStatus = (eStatus != eFrameworkunifiedStatusOK) ? eStatus : FrameworkunifiedAttachCallbacksToDispatcher(hApp,
                                                                    "SS_LoggerService",
                                                                    logging_callbacks,
                                                                    static_cast<UI_32>(_countof(logging_callbacks)));
    eStatus = FrameworkunifiedSubscribeNotificationWithCallback(hApp, NTFY_LOGGER_SETCONTROLMASK, FrameworkunifiedSetLogMask);

    // This is optional only for remote control service. Therefore return value not checked
    if (eFrameworkunifiedStatusOK != FrameworkunifiedAttachCallbacksToDispatcher(hApp, FRAMEWORKUNIFIED_NS_REMOTECONTROLSERVICE, rcs_logging_callbacks,
                                                       static_cast<UI_32>(_countof(rcs_logging_callbacks)))) {
      // LCOV_EXCL_BR_START 15:marco defined in "native_service/ns_logger_if.h"
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_WAR, __FUNCTION__,
              " FrameworkunifiedAttachCallbacksToDispatcher failed for rcs_logging_callbacks with FRAMEWORKUNIFIED_NS_REMOTECONTROLSERVICE");
      // LCOV_EXCL_BR_STOP
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }
  return eStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetLogMask(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CHANGELOGPARAMS chngLogOpts;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");

  if (FrameworkunifiedGetMsgLength(hApp) == sizeof(CHANGELOGPARAMS)) {
    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&chngLogOpts, sizeof(chngLogOpts))) {
      // Set the mask to the value that was just sent.
      NsLogSetControlMask(chngLogOpts.uiZoneMask);
    } else {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Retervial of Data Failed");
      eStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Incorrect message sz");
    eStatus = eFrameworkunifiedStatusInvldBuf;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetLogMask(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    PCSTR pAppName = FrameworkunifiedGetAppName(hApp);
    PCSTR pSrcName = FrameworkunifiedGetMsgSrc(hApp);

    if (NULL != pAppName && NULL != pSrcName) {
      HANDLE echoBack = McOpenSender(pSrcName);
      if (NULL != echoBack) {
        CHANGELOGPARAMS chng_log_params;

        NsLogGetControlMask(chng_log_params.uiZoneMask);
        if (eFrameworkunifiedStatusOK != (eStatus = McSend(echoBack, pAppName, GET_LOG_MASK_RESPONSE, sizeof(chng_log_params),
                                              &chng_log_params))) {
          FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Error sending message  GET_LOG_MASK_RESPONSE");
        }

        McClose(echoBack);
        echoBack = NULL;
      } else {
        eStatus = eFrameworkunifiedStatusInvldHandle;
      }
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetOutputLogOpts(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CHANGELOGPARAMS chngLogOpts;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");

  if (FrameworkunifiedGetMsgLength(hApp) == sizeof(CHANGELOGPARAMS)) {
    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&chngLogOpts, sizeof(chngLogOpts))) {
      // Set the logging method to the value that was just received.
      NsLogSetLogMethod(chngLogOpts.uiLogOptions);
    } else {
      FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Retervial of Data Failed");
      eStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {
    FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, " Incorrect message sz");
    eStatus = eFrameworkunifiedStatusInvldBuf;
  }


  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedNPRegisterNotification
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetOutputLogOpts(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    PCSTR pAppName = FrameworkunifiedGetAppName(hApp);
    PCSTR pSrcName = FrameworkunifiedGetMsgSrc(hApp);

    if (NULL != pAppName && NULL != pSrcName) {
      HANDLE echoBack = McOpenSender(pSrcName);

      if (NULL != echoBack) {
        CHANGELOGPARAMS chng_log_params;

        chng_log_params.uiLogOptions = NsLogGetLogMethod();
        if (eFrameworkunifiedStatusOK != (eStatus = McSend(echoBack, pAppName, GET_LOG_OUT_OPT_RESPONSE, sizeof(chng_log_params),
                                              &chng_log_params))) {
          FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Error sending message  GET_LOG_OUT_OPT_RESPONSE");
        }

        McClose(echoBack);
        echoBack = NULL;
      } else {
        eStatus = eFrameworkunifiedStatusInvldHandle;
      }
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedSetLogSeverity
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSetLogSeverity(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  CHANGELOGPARAMS l_tChngLogOpts = {};
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");

  if (FrameworkunifiedGetMsgLength(hApp) == sizeof(CHANGELOGPARAMS)) {
    if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tChngLogOpts, sizeof(l_tChngLogOpts))) {
      // Set the logging method to the value that was just received.
      NsLogSetSeverity(l_tChngLogOpts.eSeverity);
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of log severity data Failed");
      eStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size CHANGELOGPARAMS");
    eStatus = eFrameworkunifiedStatusInvldBuf;
  }

  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedGetLogSeverity
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetLogSeverity(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "+");

  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
    PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);

    if (NULL != l_cAppName && NULL != l_cSrcName) {
      HANDLE l_hEchoBack = McOpenSender(l_cSrcName);

      if (NULL != l_hEchoBack) {
        CHANGELOGPARAMS l_tChngLogParams = {};

        l_tChngLogParams.eSeverity = NsLogGetSeverity();
        if (eFrameworkunifiedStatusOK != (eStatus = McSend(l_hEchoBack, l_cAppName, GET_LOG_SEVERITY_RESPONSE,
          sizeof(l_tChngLogParams), &l_tChngLogParams))) {
          FRAMEWORKUNIFIEDLOG0(ZONE_NS_ERR, __FUNCTION__, "Error sending message  GET_LOG_OUT_OPT_RESPONSE");
        }

        McClose(l_hEchoBack);
        l_hEchoBack = NULL;
      } else {
        eStatus = eFrameworkunifiedStatusInvldHandle;
      }
    } else {
      eStatus = eFrameworkunifiedStatusNullPointer;
    }
  } else {
    eStatus = eFrameworkunifiedStatusInvldParam;
  }

  FRAMEWORKUNIFIEDLOG0(ZONE_NS_INFO, __FUNCTION__, "-");
  return eStatus;
}
// LCOV_EXCL_STOP

////////////////////////////////////////////////////////////////////////////////////////////
/// FrameworkunifiedDebugDumpRequest: Calls an Applications FrameworkunifiedOnDebugDump method.
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDebugDumpRequest(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DEBUG_DUMP, __FUNCTION__, "+");
  eStatus = FrameworkunifiedOnDebugDumpInternal(hApp);
  FRAMEWORKUNIFIEDLOG0(ZONE_NS_DEBUG_DUMP, __FUNCTION__, "+");
  return eStatus;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus NSRcsSetLogSettings(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(TNSRCS_SetLogSettingsReq)) {
      TNSRCS_SetLogSettingsReq l_tSetLogSettings = {};

      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tSetLogSettings, sizeof(l_tSetLogSettings))) {
        // Set the log mask
        NsLogSetControlMask(l_tSetLogSettings.m_pui32ZoneMask);
        // set the log output option
        NsLogSetLogMethod(l_tSetLogSettings.m_ui8OutputLogOption);
        // set the log severity
        NsLogSetSeverity(l_tSetLogSettings.m_si32Severity);
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Log settings applied");

        // send the response back with the current settings
        TNSRCS_SetLogSettingsResp l_tSetLogSettingsResp = {};
        PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
        PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);
        HANDLE l_hSendTo = McOpenSender(l_cSrcName);

        if (NULL != l_hSendTo) {
          l_tSetLogSettingsResp.m_ui8ClientId = l_tSetLogSettings.m_ui8ClientId;
          l_tSetLogSettingsResp.m_ui8SetStatus = static_cast<UI_8>(eFrameworkunifiedStatusOK);

          NsLogGetControlMask(l_tSetLogSettingsResp.m_pui32ZoneMask);
          l_tSetLogSettingsResp.m_ui8OutputLogOption = NsLogGetLogMethod();
          l_tSetLogSettingsResp.m_si32Severity = NsLogGetSeverity();

          if (eFrameworkunifiedStatusOK !=
              McSend(l_hSendTo, l_cAppName, NSRCS_SET_LOG_SETTINGS_RESP,
                     sizeof(TNSRCS_SetLogSettingsResp), &l_tSetLogSettingsResp)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending message  "
                   "NSRCS_SET_LOG_SETTINGS_RESP to %s", l_cSrcName);
          }
          McClose(l_hSendTo);
          l_hSendTo = NULL;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Sender handle NULL when sending response to %s", l_cSrcName);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of Data Failed");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size. Received: %d expected: %ld",
             FrameworkunifiedGetMsgLength(hApp),
             static_cast<long int>(sizeof(TNSRCS_SetLogSettingsReq)));  // NOLINT  (readability/nolint)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus NSRcsGetLogSettings(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(TNSRCS_GetLogSettingsReq)) {
      TNSRCS_GetLogSettingsReq l_tGetLogSettings = {};

      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tGetLogSettings, sizeof(l_tGetLogSettings))) {
        // send the response back with the current settings
        TNSRCS_GetLogSettingsResp l_tGetLogSettingsResp = {};
        PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
        PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);
        HANDLE l_hSendTo = McOpenSender(l_cSrcName);

        if (NULL != l_hSendTo) {
          l_tGetLogSettingsResp.m_ui8ClientId = l_tGetLogSettings.m_ui8ClientId;
          NsLogGetZoneTextList(l_tGetLogSettingsResp.m_cZoneList);
          NsLogGetControlMask(l_tGetLogSettingsResp.m_pui32ZoneMask);
          l_tGetLogSettingsResp.m_ui8OutputLogOption = NsLogGetLogMethod();
          l_tGetLogSettingsResp.m_si32Severity = NsLogGetSeverity();

          if (eFrameworkunifiedStatusOK !=
              McSend(l_hSendTo, l_cAppName, NSRCS_GET_LOG_SETTINGS_RESP,
                     sizeof(TNSRCS_GetLogSettingsResp), &l_tGetLogSettingsResp)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending message  "
                   "NSRCS_GET_LOG_SETTINGS_RESP to %s", l_cSrcName);
          }
          McClose(l_hSendTo);
          l_hSendTo = NULL;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Sender handle NULL when sending response to %s", l_cSrcName);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of Data Failed");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size. Received: %d expected: %ld",
             FrameworkunifiedGetMsgLength(hApp),
             static_cast<long int>(sizeof(TNSRCS_GetLogSettingsReq)));  // NOLINT  (readability/nolint)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus NSRcsSetLogMask(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(TNSRCS_SetLogSettingsReq)) {
      TNSRCS_SetLogSettingsReq l_tSetLogSettings = {};

      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tSetLogSettings, sizeof(l_tSetLogSettings))) {
        // Set the log mask
        NsLogSetControlMask(l_tSetLogSettings.m_pui32ZoneMask);

        // 11 = number of chars in UI_32, 1 - nul char at the end
        CHAR l_cOutMask[(ZONE_MASK_ARRAY_ELTS * 11) + 1] = {};
        CHAR l_cMask[12] = {};  // 12 = 11+1: 11 = number of chars in UI_32, 1 - nul char at the end
        for (UI_32 zoneIndex = 0; zoneIndex < ZONE_MASK_ARRAY_ELTS; zoneIndex++) {
          std::strncpy(l_cMask, "", 11);
          snprintf(l_cMask, sizeof(l_cMask), "0x%X,", l_tSetLogSettings.m_pui32ZoneMask[zoneIndex]);
          std::strncat(l_cOutMask, l_cMask, (sizeof(l_cOutMask) - std::strlen(l_cMask) - 1));
        }
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Log settings (zone mask) applied: %s", l_cOutMask);

        // send the response back with the current settings
        TNSRCS_SetLogMaskResp l_tSetLogMaskResp = {};
        PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
        PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);
        HANDLE l_hSendTo = McOpenSender(l_cSrcName);

        if (NULL != l_hSendTo) {
          l_tSetLogMaskResp.m_ui8ClientId = l_tSetLogSettings.m_ui8ClientId;
          l_tSetLogMaskResp.m_ui8SetStatus = static_cast<UI_8>(eFrameworkunifiedStatusOK);

          NsLogGetControlMask(l_tSetLogMaskResp.m_pui32ZoneMask);

          if (eFrameworkunifiedStatusOK !=
              McSend(l_hSendTo, l_cAppName, NSRCS_SET_LOG_MASK_RESP,
                     sizeof(TNSRCS_SetLogMaskResp), &l_tSetLogMaskResp)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending message  "
                   "NSRCS_SET_LOG_MASK_RESP to %s", l_cSrcName);
          }
          McClose(l_hSendTo);
          l_hSendTo = NULL;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Sender handle NULL when sending response to %s", l_cSrcName);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of Data Failed");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size. Received: %d expected: %ld",
             FrameworkunifiedGetMsgLength(hApp),
             static_cast<long int>(sizeof(TNSRCS_SetLogSettingsReq)));  // NOLINT  (readability/nolint)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus NSRcsSetOutputLogOpts(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(TNSRCS_SetLogSettingsReq)) {
      TNSRCS_SetLogSettingsReq l_tSetLogSettings = {};

      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tSetLogSettings, sizeof(l_tSetLogSettings))) {
        // set the log output option
        NsLogSetLogMethod(l_tSetLogSettings.m_ui8OutputLogOption);
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Log settings(log output option) applied: %d.",
               l_tSetLogSettings.m_ui8OutputLogOption);

        // send the response back with the current settings
        TNSRCS_SetLogOutOptResp l_tSetLogOutputOptResp = {};
        PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
        PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);
        HANDLE l_hSendTo = McOpenSender(l_cSrcName);

        if (NULL != l_hSendTo) {
          l_tSetLogOutputOptResp.m_ui8ClientId = l_tSetLogSettings.m_ui8ClientId;
          l_tSetLogOutputOptResp.m_ui8SetStatus = static_cast<UI_8>(eFrameworkunifiedStatusOK);

          l_tSetLogOutputOptResp.m_ui8OutputLogOption = NsLogGetLogMethod();

          if (eFrameworkunifiedStatusOK !=
              McSend(l_hSendTo, l_cAppName, NSRCS_SET_LOG_OUT_OPT_RESP,
                     sizeof(TNSRCS_SetLogOutOptResp), &l_tSetLogOutputOptResp)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending message  "
                   "NSRCS_SET_LOG_OUT_OPT_RESP to %s", l_cSrcName);
          }
          McClose(l_hSendTo);
          l_hSendTo = NULL;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Sender handle NULL when sending response to %s", l_cSrcName);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of Data Failed");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size. Received: %d expected: %ld",
             FrameworkunifiedGetMsgLength(hApp),
             static_cast<long int>(sizeof(TNSRCS_SetLogSettingsReq)));  // NOLINT  (readability/nolint)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus NSRcsSetLogSeverity(HANDLE hApp) {  // LCOV_EXCL_START 7:debug code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "+");
  if (frameworkunifiedCheckValidAppHandle(hApp)) {
    if (FrameworkunifiedGetMsgLength(hApp) == sizeof(TNSRCS_SetLogSettingsReq)) {
      TNSRCS_SetLogSettingsReq l_tSetLogSettings = {};

      if (eFrameworkunifiedStatusOK == FrameworkunifiedGetMsgDataOfSize(hApp, (PVOID)&l_tSetLogSettings, sizeof(l_tSetLogSettings))) {
        // set the log severity
        NsLogSetSeverity(l_tSetLogSettings.m_si32Severity);
        FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "Log settings(log severity) applied: %d.",
          l_tSetLogSettings.m_si32Severity);

        // send the response back with the current settings
        TNSRCS_SetLogSeverityResp l_tSetLogSeverityResp = {};
        PCSTR l_cAppName   = FrameworkunifiedGetAppName(hApp);
        PCSTR l_cSrcName   = FrameworkunifiedGetMsgSrc(hApp);
        HANDLE l_hSendTo = McOpenSender(l_cSrcName);

        if (NULL != l_hSendTo) {
          l_tSetLogSeverityResp.m_ui8ClientId = l_tSetLogSettings.m_ui8ClientId;
          l_tSetLogSeverityResp.m_ui8SetStatus = static_cast<UI_8>(eFrameworkunifiedStatusOK);

          l_tSetLogSeverityResp.m_si32Severity = NsLogGetSeverity();

          if (eFrameworkunifiedStatusOK !=
              McSend(l_hSendTo, l_cAppName, NSRCS_SET_LOG_SEVERITY_RESP,
                     sizeof(TNSRCS_SetLogSeverityResp), &l_tSetLogSeverityResp)) {
            FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Error sending message  "
                   "NSRCS_SET_LOG_SEVERITY_RESP to %s", l_cSrcName);
          }
          McClose(l_hSendTo);
          l_hSendTo = NULL;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Sender handle NULL when sending response to %s", l_cSrcName);
        }
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "Retrieval of Data Failed");
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, " Incorrect message size. Received: %d expected: %ld",
      FrameworkunifiedGetMsgLength(hApp),
      static_cast<long int>(sizeof(TNSRCS_SetLogSettingsReq)));  // NOLINT  (readability/nolint)
    }
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __FUNCTION__, "hApp is NULL");
  }
  FRAMEWORKUNIFIEDLOG(ZONE_NS_INFO, __FUNCTION__, "-");
  return eFrameworkunifiedStatusOK;
}
// LCOV_EXCL_STOP
