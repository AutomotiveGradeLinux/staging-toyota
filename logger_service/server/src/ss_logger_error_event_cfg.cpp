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
/// \ingroup  tag_SS_LoggerService
/// \brief    This file supports error event logging configuration.
///
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <native_service/frameworkunified_framework_if.h>
#include <system_service/ss_logger_service.h>
#include <system_service/ss_services.h>
#include <system_service/ss_templates.h>
#include <vector>
#include "loggerservicedebug_loggerservicelog.h"
#include "ss_logger_error_event_cfg.h"

enum ARTIFACT_BIT {
  ARTIFACT_BIT_NONE = 0x00000000,
  ARTIFACT_BIT_LOGGERSERVICE_DEBUG_LOG = 1 << (UI_32) eArtifactIdInterfaceunifiedDebugLog,
  ARTIFACT_BIT_TRANSMIT_LOG = 1 << (UI_32) eArtifactIdTransmitLog,
  ARTIFACT_BIT_PERFORMANCE_LOG = 1 << (UI_32) eArtifactIdPerformanceLog,
  ARTIFACT_BIT_BOOT_MICRO_LOG = 1 << (UI_32) eArtifactIdBootMicroLog,
  ARTIFACT_BIT_SYSTEM_DATA_LOG = 1 << (UI_32) eArtifactIdSystemDataCsv,
  ARTIFACT_BIT_SHOW_MEM_TXT = 1 << (UI_32) eArtifactIdShowMemTxt,
  ARTIFACT_BIT_PROCESS_CORE = 1 << (UI_32) eArtifactIdProcessCore,
  ARTIFACT_BIT_DEBUG_DUMP_LOG = 1 << (UI_32) eArtifactIdDebugDumpLog,
  ARTIFACT_BIT_KERNEL_LOG = 1 << (UI_32) eArtifactIdKernelLog,
  ARTIFACT_BIT_KERNEL_BOOT_LOG = 1 << (UI_32) eArtifactIdKernelBootLog,
  ARTIFACT_BIT_DRINITIAL_LOG = 1 << (UI_32) eArtifactIdDRInitialLog,
  ARTIFACT_BIT_DRLOCATION_LOG = 1 << (UI_32) eArtifactIdDRLocationLog,
  ARTIFACT_BIT_CHLM_MC_LOGS = 1 << (UI_32) eArtifactIdCpuHighLoadMonteCarloLogs,
  ARTIFACT_BIT_METACORE_LOGS = 1 << (UI_32) eArtifactIdMetaCoreLogs,
  ARTIFACT_BIT_SCREEN_SHOT = 1 << (UI_32) eArtifactIdScreenShot,
  ARTIFACT_BIT_CMS_LOGS = 1 << (UI_32) eArtifactIdCmsLogs,
  ARTIFACT_BIT_INTERNAL_DTC_LOG = 1 << (UI_32) eArtifactIDInternalDTC,
  ARTIFACT_BIT_CLEAR_ALL_LOG = 1 << (UI_32) eArtifactIdClearAllLog,
  ARTIFACT_BIT_NAVI_LOG = 1 << (UI_32) eArtifactIdNaviLog,
  ARTIFACT_BIT_DEBUG_FOLDER_CONTENT = 1
      << (UI_32) eArtifactIdDebugFolderContent,
  ARTIFACT_BIT_DEBUG_FOLDER2_CONTENT = 1
      << (UI_32) eArtifactIdDebugFolder2Content,
  ARTIFACT_BIT_COM_LOG = 1 << (UI_32) eArtifactIdComDebugLog,
  ARTIFACT_BIT_RADIO_LOG = 1 << (UI_32) eArtifactIdRadioDebugLog,
  ARTIFACT_BIT_CONNECTIVITY_LOG = 1 << (UI_32) eArtifactIdConnectivityDebugLog,
  ARTIFACT_BIT_GRAPHICS_LOG = 1 << (UI_32) eArtifactIdGraphicsDebugLog,
  ARTIFACT_BIT_SYS_LOG = 1 << (UI_32) eArtifactIdSysLog,
  ARTIFACT_BIT_PSTORE_LOG = 1 << (UI_32) eArtifactIdPstoreLog,
  ARTIFACT_BIT_WINSYS_LOG = 1 << (UI_32) eArtifactIdWinSysLog,
  ARTIFACT_BIT_APPFW_LOG = 1 << (UI_32) eArtifactIdAppFwLog,
  ARTIFACT_BIT_TOMOYO_LOG = 1 << (UI_32) eArtifactIdTomoyoLog
};

//////////////////////////////////////////////////////////////////////////////////////////
/////  Definition for Logging Artifact Filepaths
//////////////////////////////////////////////////////////////////////////////////////////
const char LAF_DEBUG_FOLDER[] = "/nv/log";
const char LAF_DEBUG_FOLDER2[] = "/nv/log2";
const char LAF_MONTECARLO_FN[] = "/tmp/MonteCarlo";
const char LAF_METACORE_FN[] = "/tmp/MetaCore";
const char LAF_CMS_FN[] = "/tmp/cms";
const char LAF_INT_DTC_FN[] = "/tmp/dtc_error.log";
const char LAF_LOGGERSERVICELOG_COM[] = "/ramd/log/frameworkunifiedlog/communication";
const char LAF_LOGGERSERVICELOG_RDO[] = "/ramd/log/frameworkunifiedlog/radio";
const char LAF_LOGGERSERVICELOG_CONNECTIVITY[] = "/ramd/log/frameworkunifiedlog/connectivity";
const char LAF_GRAPHICS_FN[] = "/ramd/log/frameworkunifiedlog/graphics.log";
const char LAF_KERNEL_BOOT_FN[] = "/ramd/log/frameworkunifiedlog/kernelboot.log";
const char LAF_SYSLOG[] = "/ramd/log/frameworkunifiedlog/syslog";
const char LAF_PSTORELOG[] = "/dev/pstore";
const char LAF_WINSYS_FN[] = "/ramd/log/frameworkunifiedlog/winsys.log";
const char LAF_APPFW[] = "/ramd/log/frameworkunifiedlog/appfw";
const char LAF_TOMOYO[] = "/nv/ptdata/sec/tomoyo";

#define ARTIFACT_BIT_ALL_OUTPUT_LOG  (ARTIFACT_BIT_LOGGERSERVICE_DEBUG_LOG |  \
              ARTIFACT_BIT_DEBUG_DUMP_LOG |  \
              ARTIFACT_BIT_SYSTEM_DATA_LOG |  \
              ARTIFACT_BIT_SHOW_MEM_TXT |  \
              ARTIFACT_BIT_COM_LOG |    \
              ARTIFACT_BIT_RADIO_LOG |    \
              ARTIFACT_BIT_CONNECTIVITY_LOG |  \
              ARTIFACT_BIT_KERNEL_LOG |    \
                ARTIFACT_BIT_KERNEL_BOOT_LOG  | \
              ARTIFACT_BIT_SCREEN_SHOT |  \
              ARTIFACT_BIT_TRANSMIT_LOG |  \
              ARTIFACT_BIT_PERFORMANCE_LOG |  \
              ARTIFACT_BIT_DRINITIAL_LOG |  \
              ARTIFACT_BIT_DRLOCATION_LOG |  \
              ARTIFACT_BIT_CHLM_MC_LOGS |  \
              ARTIFACT_BIT_INTERNAL_DTC_LOG |  \
              ARTIFACT_BIT_GRAPHICS_LOG |  \
              ARTIFACT_BIT_SYS_LOG |    \
              ARTIFACT_BIT_METACORE_LOGS |      \
              ARTIFACT_BIT_PSTORE_LOG    |      \
              ARTIFACT_BIT_WINSYS_LOG    |      \
                ARTIFACT_BIT_APPFW_LOG |    \
              ARTIFACT_BIT_TOMOYO_LOG)

TLoggingArtifactCfg CErrorEventCfg::m_loggerErrorEventCfgTbl[eErrorEventTypeMaxValue] = {
    { eErrorEventTypeProcessCrash, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeProcessExit, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeHeartBeatFailure, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeSystemLowMemory, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeUserInvokedUserForceReset, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_NONE) } },

    { eErrorEventTypeUserInvokedCollectAllLogs, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_POPUP | EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeUserInvokedCollectScreenShot, { (ARTIFACT_BIT_NONE
        | ARTIFACT_BIT_SCREEN_SHOT),

    (EVENT_BIT_POPUP | EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_POPUP | EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeUserInvokedCollectDevLogs, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_NONE) }
    },

    { eErrorEventTypeBootMicroReset, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeEelExport, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_POPUP | EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeInterfaceunifiedEmmcLogs, { (ARTIFACT_BIT_DEBUG_FOLDER_CONTENT
        | ARTIFACT_BIT_DEBUG_FOLDER2_CONTENT),

    (EVENT_BIT_POPUP | EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeDiagEvent, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_DIAG) } },

    { eErrorEventTypeCanEvent, { ARTIFACT_BIT_ALL_OUTPUT_LOG,
    (EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeDtcEvent, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG),

    (EVENT_BIT_CAN_NOTIFICATION) } },

    { eErrorEventTypeModConnFailed, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeStartRespFailed, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeModuleInvokedResetRequest, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeModuleInvokedCollectDebugLogs,
        { (ARTIFACT_BIT_ALL_OUTPUT_LOG | ARTIFACT_BIT_NAVI_LOG),
            (EVENT_BIT_NONE) } },

    { eErrorEventTypeUserInvokedClearLogs, { (ARTIFACT_BIT_NONE
        | ARTIFACT_BIT_CLEAR_ALL_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeUserInvokedCollectNaviLog, { (ARTIFACT_BIT_NONE
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeGroupRelaunch, { (ARTIFACT_BIT_ALL_OUTPUT_LOG
        | ARTIFACT_BIT_NAVI_LOG), (EVENT_BIT_NONE) } },

    { eErrorEventTypeReserved1, { (ARTIFACT_BIT_NONE), (EVENT_BIT_NONE) } },

    { eErrorEventTypeReserved2, { (ARTIFACT_BIT_NONE), (EVENT_BIT_NONE) } },

    { eErrorEventTypeReserved3, { (ARTIFACT_BIT_NONE), (EVENT_BIT_NONE) } } };

TLoggingArtifact CErrorEventCfg::m_loggerErrorArtifactCfgTbl[eArtifactIdMaxValue] = {  // LCOV_EXCL_BR_LINE 11:Unexpected branch
    /*Artifact ID    Owner    Timeout Callback Type    Path CallBack Function    Delete tmp artifact*/
    { eArtifactIdInterfaceunifiedDebugLog, { SERVICE_LOGGER, 5000, { eCbTypeFunction, "",
        SSLogger_SendtoSelf }, TRUE } }, { eArtifactIdTransmitLog,
        { SERVICE_LOGGER, 5000, { eCbTypeFunction, "", SSLogger_SendtoSelf },
            TRUE } }, { eArtifactIdPerformanceLog, { SERVICE_LOGGER, 5, {
        eCbTypeFunction, "", SSLogger_SendtoSelf }, FALSE } }, {
        eArtifactIdBootMicroLog, { SERVICE_SYSMANAGER, 1000, { eCbTypeFunction,
            "", SSLogger_SendtoSM }, TRUE } }, { eArtifactIdSystemDataCsv, {
        SERVICE_SYSMANAGER, 5000, { eCbTypeFunction, "", SSLogger_SendtoSM },
        TRUE } }, { eArtifactIdShowMemTxt, { SERVICE_SYSMANAGER, 8000, {
        eCbTypeFunction, "", SSLogger_SendtoSM }, TRUE } }, {
        eArtifactIdProcessCore, { SERVICE_SYSMANAGER, 5000, { eCbTypeFunction,
            "", SSLogger_SendtoSM }, TRUE } }, { eArtifactIdDebugDumpLog, {
        SERVICE_SYSMANAGER, 4000, { eCbTypeFunction, "", SSLogger_SendtoSM },
        TRUE } }, { eArtifactIdScreenShot, { SERVICE_LOGGER, 1100, {
        eCbTypeFunction, "", SSLogger_SendtoSelf }, TRUE } }, {
        eArtifactIdDebugFolderContent, { SERVICE_LOGGER, 50000, { eCbTypePath,
            LAF_DEBUG_FOLDER, NULL }, FALSE } }, {
        eArtifactIdDebugFolder2Content, { SERVICE_LOGGER, 50000, { eCbTypePath,
            LAF_DEBUG_FOLDER2, NULL }, FALSE } }, { eArtifactIdKernelLog,
        { SERVICE_LOGGER, 5000, { eCbTypeFunction, "", SSLogger_SendtoSelf },
            TRUE } }, { eArtifactIdKernelBootLog, { SERVICE_LOGGER, 5000, {
        eCbTypePath, LAF_KERNEL_BOOT_FN, NULL }, FALSE } }, {
        eArtifactIdDRInitialLog, { SERVICE_LOGGER, 500, { eCbTypeFunction, "",
            SSLogger_SendtoSelf }, TRUE } }, { eArtifactIdDRLocationLog,
        { SERVICE_LOGGER, 500, { eCbTypeFunction, "", SSLogger_SendtoSelf },
            TRUE } }, { eArtifactIdCpuHighLoadMonteCarloLogs, { SERVICE_LOGGER,
        50, { eCbTypePath, LAF_MONTECARLO_FN, NULL }, TRUE } }, {
        eArtifactIdMetaCoreLogs, { SERVICE_LOGGER, 50, { eCbTypePath,
            LAF_METACORE_FN, NULL }, TRUE } }, { eArtifactIdCmsLogs, {
        SERVICE_LOGGER, 500, { eCbTypePath, LAF_CMS_FN, NULL }, TRUE } }, {
        eArtifactIDInternalDTC, { SERVICE_LOGGER, 500, { eCbTypePath,
            LAF_INT_DTC_FN, NULL }, TRUE } }, { eArtifactIdComDebugLog, {
        SERVICE_LOGGER, 500, { eCbTypePath, LAF_LOGGERSERVICELOG_COM, NULL }, FALSE } }, {
        eArtifactIdRadioDebugLog, { SERVICE_LOGGER, 500, { eCbTypePath,
            LAF_LOGGERSERVICELOG_RDO, NULL }, FALSE } }, {
        eArtifactIdConnectivityDebugLog, { SERVICE_LOGGER, 500, { eCbTypePath,
            LAF_LOGGERSERVICELOG_CONNECTIVITY, NULL }, FALSE } }, {
        eArtifactIdGraphicsDebugLog, { SERVICE_LOGGER, 50, { eCbTypePath,
            LAF_GRAPHICS_FN, NULL }, FALSE } }, { eArtifactIdSysLog, {
        SERVICE_LOGGER, 50, { eCbTypePath, LAF_SYSLOG, NULL }, FALSE } }, {
        eArtifactIdPstoreLog, { SERVICE_LOGGER, 50, { eCbTypePath,
            LAF_PSTORELOG, NULL }, FALSE } }, { eArtifactIdClearAllLog,
        { SERVICE_LOGGER, 500, { eCbTypeFunction, "", SSLogger_SendtoSelf },
            FALSE } }, { eArtifactIdNaviLog, { SERVICE_LOGGER, 5000, {
        eCbTypeFunction, "", SSLogger_SendtoSelf }, FALSE } }, {
        eArtifactIdWinSysLog, { SERVICE_LOGGER, 50, { eCbTypePath,
            LAF_WINSYS_FN, NULL }, FALSE } }, { eArtifactIdAppFwLog, {
        SERVICE_LOGGER, 500, { eCbTypePath, LAF_APPFW, NULL }, FALSE } }, {
        eArtifactIdTomoyoLog, { SERVICE_LOGGER, 50, { eCbTypePath, LAF_TOMOYO,
            NULL }, TRUE } } };  // LCOV_EXCL_BR_LINE 11:Unexpected branch

CErrorEventCfg::CErrorEventCfg() {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

CErrorEventCfg::~CErrorEventCfg() {  // LCOV_EXCL_START 14:globle instance
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}
// LCOV_EXCL_STOP

EFrameworkunifiedStatus CErrorEventCfg::Initialize(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EFrameworkunifiedStatus l_eStatus = eFrameworkunifiedStatusOK;
  UI_32 l_cnt;

  for (l_cnt = 0; l_cnt < eArtifactIdMaxValue; l_cnt++) {
    EArtifactId l_artifactId = m_loggerErrorArtifactCfgTbl[l_cnt].ArtifactId;
    TLoggingArtifactInformation l_artifactInformation =
        m_loggerErrorArtifactCfgTbl[l_cnt].Information;
    m_errorArtifactMap[l_artifactId] = l_artifactInformation;  // LCOV_EXCL_BR_LINE 11:Unexpected branch
  }
  for (l_cnt = 0; l_cnt < eErrorEventTypeMaxValue; l_cnt++) {
    EErrorEventType l_eventType = m_loggerErrorEventCfgTbl[l_cnt].ErrorType;
    TLoggingErrorEventInformation l_errorEvtInformation =
        m_loggerErrorEventCfgTbl[l_cnt].Information;
    m_errorEventTypeToArtifactBitMaskMap[l_eventType] = l_errorEvtInformation;
  }

  l_eStatus = (EFrameworkunifiedStatus) ValidateConfiguration();
  LOG_STATUS_IF_ERRORED(l_eStatus, "ValidateConfiguration()");  // LCOV_EXCL_BR_LINE 5:macro

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_eStatus);
}

UI_32 CErrorEventCfg::ValidateConfiguration(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");

  UI_32 l_return = 0;
  if (eArtifactIdMaxValue != m_errorArtifactMap.size()) {  // LCOV_EXCL_BR_LINE 6:Be sure not to exceed the eArtifactIdMaxValue
    // LCOV_EXCL_START 6:Be sure not to exceed the eArtifactIdMaxValue
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Artifact map size mismatch: Expected: %d, Found: %d",
           eArtifactIdMaxValue, m_errorArtifactMap.size());
    l_return |= 1;
    // LCOV_EXCL_STOP
  }
  if (eErrorEventTypeMaxValue != m_errorEventTypeToArtifactBitMaskMap.size()) {  // LCOV_EXCL_BR_LINE 6:Be sure not to exceed the eErrorEventTypeMaxValue
    // LCOV_EXCL_START 6:Be sure not to exceed the eErrorEventTypeMaxValue
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "Error event map size mismatch: Expected: %d, Found: %d",
           eErrorEventTypeMaxValue, m_errorEventTypeToArtifactBitMaskMap.size());
    l_return |= 4;
    // LCOV_EXCL_STOP
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_return);
}

void CErrorEventCfg::GetArtifactRequestVec(
    EErrorEventType f_eventType,
    std::vector<TLoggingArtifact> &f_refArtifactRequestVec) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  ARTIFACT_BIT_MASK l_artifactRequestBits = 0;
  ARTIFACT_BIT l_artifactBit;
  TLoggingArtifact l_artifact;
  UI_32 l_artifactId;

  f_refArtifactRequestVec.clear();

  if (f_eventType < eErrorEventTypeMaxValue) {
    l_artifactRequestBits = m_errorEventTypeToArtifactBitMaskMap[f_eventType]
        .ArtifactBitMask;
  }

  if (l_artifactRequestBits > 0) {
    for (l_artifactId = 0; l_artifactId < eArtifactIdMaxValue; l_artifactId++) {
      l_artifactBit = (ARTIFACT_BIT) (1 << l_artifactId);
      if ((l_artifactRequestBits & l_artifactBit) > 0) {
        l_artifact.ArtifactId = (EArtifactId) l_artifactId;
        l_artifact.Information = m_errorArtifactMap[l_artifact.ArtifactId];
        f_refArtifactRequestVec.push_back(l_artifact);
      }
    }
  }

  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
}

EVENT_BIT_MASK CErrorEventCfg::GetEventsForErrorEvent(
    EErrorEventType f_eventType) {
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "+");
  EVENT_BIT_MASK l_events = EVENT_BIT_NONE;
  if (f_eventType < eErrorEventTypeMaxValue) {  // LCOV_EXCL_BR_LINE 6:Be sure not to exceed the eErrorEventTypeMaxValue
    l_events = m_errorEventTypeToArtifactBitMaskMap[f_eventType].EventBitMask;
  }
  FRAMEWORKUNIFIEDLOG(ZONE_FUNC, __FUNCTION__, "-");
  return (l_events);
}  // LCOV_EXCL_BR_LINE 10:the last line
