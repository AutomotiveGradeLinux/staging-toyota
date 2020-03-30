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

/**
 * @file ss_error_event_common.h
 * @brief This file supports SM logging.
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup interface_unified
 *  @ingroup system_service
 *  @{
 */

#ifndef __SS_ERROR_EVENT_COMMON_H__  // NOLINT (build/header_guard)
#define __SS_ERROR_EVENT_COMMON_H__

#include <native_service/frameworkunified_types.h>  // added for test build


// Do not change enumeration order.
// HMI hard codes error event square
// colors based on expected enum order.
enum EErrorEventType {
    eErrorEventTypeProcessCrash = 0,
    eErrorEventTypeHeartBeatFailure,
    eErrorEventTypeSystemLowMemory,
    eErrorEventTypeReserved1,  // Formerly SS_SM_EVENT_ERROR_HIGH_CPU_LOAD, now HMI enum place holder.
    eErrorEventTypeUserInvokedUserForceReset,
    eErrorEventTypeReserved2,
    eErrorEventTypeReserved3,
    eErrorEventTypeUserInvokedCollectAllLogs,
    eErrorEventTypeBootMicroReset,
    eErrorEventTypeProcessExit,
    eErrorEventTypeUserInvokedCollectScreenShot,
    eErrorEventTypeUserInvokedCollectInterfaceunifiedLogs,
    eErrorEventTypeEelExport,
    eErrorEventTypeInterfaceunifiedEmmcLogs,
    eErrorEventTypeDiagEvent,
    eErrorEventTypeCanEvent,
    eErrorEventTypeDtcEvent,
    eErrorEventTypeModConnFailed,
    eErrorEventTypeStartRespFailed,
    eErrorEventTypeUserInvokedCollectDevLogs,
    eErrorEventTypeModuleInvokedResetRequest,
    eErrorEventTypeModuleInvokedCollectDebugLogs,
    eErrorEventTypeUserInvokedClearLogs,
    eErrorEventTypeUserInvokedCollectNaviLog,
    eErrorEventTypeGroupRelaunch,
    eErrorEventTypeMaxValue
};

/* Value range of EArtifactId must be defined within 32, */
/* because of it's definition is used by 4byte bitmask.  */
enum EArtifactId {
    eArtifactIdInterfaceunifiedDebugLog = 0,
    eArtifactIdTransmitLog,
    eArtifactIdPerformanceLog,
    eArtifactIdBootMicroLog,
    eArtifactIdSystemDataCsv,
    eArtifactIdShowMemTxt,              // << ID 5
    eArtifactIdProcessCore,
    eArtifactIdDebugDumpLog,
    eArtifactIdKernelLog,
    eArtifactIdDRInitialLog,
    eArtifactIdDRLocationLog,           // << ID 10
    eArtifactIdCpuHighLoadMonteCarloLogs,
    eArtifactIdMetaCoreLogs,
    eArtifactIdCmsLogs,
    eArtifactIDInternalDTC,
    eArtifactIdComDebugLog,             // << ID 15
    eArtifactIdRadioDebugLog,
    eArtifactIdConnectivityDebugLog,
    eArtifactIdKernelBootLog,
    eArtifactIdGraphicsDebugLog,
    eArtifactIdSysLog,
    eArtifactIdPstoreLog,
    eArtifactIdClearAllLog,
    eArtifactIdNaviLog,
    eArtifactIdWinSysLog,
    eArtifactIdAppFwLog,
    eArtifactIdTomoyoLog,
    eArtifactIdScreenShot,              // These three Log entries have to remain
    eArtifactIdDebugFolder2Content,     // at the end of the list
    eArtifactIdDebugFolderContent,
    eArtifactIdMaxValue                 // << 30
};

struct ARTIFACT_RESPONSE {
  EArtifactId  ArtifactId;
    CHAR         FilePathAndName[MAX_PATH_LENGTH];
};


#endif /* __SS_ERROR_EVENT_COMMON_H___ */  // NOLINT (build/header_guard)

/** @}*/
/** @}*/
/** @}*/
