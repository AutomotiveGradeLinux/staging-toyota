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

/**
 * @file ss_error_event_common_local.h
 * @brief  This file supports SM logging.
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

#ifndef __SS_ERROR_EVENT_COMMON_LOCAL_H__  // NOLINT (build/header_guard)
#define __SS_ERROR_EVENT_COMMON_LOCAL_H__

#define SS_ERROR_EVENT_START_REQ_TO_SEC               (5)
#define SS_ERROR_EVENT_DEBUG_DUMP_RSPN_TO_SEC         (1)
#define SS_ERROR_EVENT_BOOT_MICRO_LOG_RESPONSE_TO_SEC (2)
#define SS_ERROR_EVENT_CORE_FILE_POLL_TO_MS           (100)

// Do not change enumeration order.
// HMI hard codes error event square
// colors based on expected enum order.
enum EErrorEventPrio {                                            /* Higher number = higher priority.       */
    eErrorEventPrioDiagLogRequest                = 8,
    eErrorEventPrioUserInvokedCollectAllLogs     = 7,
    eErrorEventPrioUserInvokedCollectScreenShot  = 6,
    eErrorEventPrioUserInvokedCollectInterfaceunifiedLogs     = 5,
    eErrorEventPrioModuleInvokedCollectDebugLogs = 4,
    eErrorEventPrioUserInvokedUserForceReset     = 3,
    eErrorEventPrioEelStorageFilePresent         = 2,
    eErrorEventPrioEmmcLogsFilePresent           = 1,
    eErrorEventPrioDefault                       = 0
};


#endif /* __SS_ERROR_EVENT_COMMON_LOCAL_H__ */  // NOLINT (build/header_guard)

/** @}*/
/** @}*/
/** @}*/

