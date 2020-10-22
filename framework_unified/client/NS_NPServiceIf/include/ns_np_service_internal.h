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
 * @file ns_np_service_internal.h
 * @brief \~english TestFramework provides interface for implementing functional test classes.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_INTERNAL_H_

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/// SendStopToNSNPP
/// To be used by System Manager Service only. This API would push NPP Service to persist
/// the data/files. Ideally this should be sent during system shutdown by System Manager Service
///
/// \param [in] hNPMsgQ
///         HANDLE - Handle to message queue of Notification service.
/// \param [in] pPublisherName
///         PCSTR - Name of the requesters message queue
/// \param [in] eShutdownType
///         EFrameworkunifiedShutdownType - shutdown type
/// \param [in] uiStopMsgData
///         UI_32 - Flag representing which data to persist and which data to delete
///         from persistent memory
///
/// \return status
///         EFrameworkunifiedStatus - success or error
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendStopToNSNPP(HANDLE hNPMsgQ, PCSTR pPublisherName, EFrameworkunifiedShutdownType eShutdownType, UI_32 uiStopMsgData);


#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_NPSERVICEIF_INCLUDE_NS_NP_SERVICE_INTERNAL_H_
/** @}*/
/** @}*/
/** @}*/

// EOF
