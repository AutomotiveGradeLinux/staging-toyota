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
 * @file ss_sm_client_if_local.h
 * @brief \~english This file supports the api of debug dump response to System Manager
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
#ifndef __SS_SYSTEM_MANAGER_CLIENT_IF_LOCAL_H__  // NOLINT (build/header_guard)
#define __SS_SYSTEM_MANAGER_CLIENT_IF_LOCAL_H__

#include <native_service/frameworkunified_framework_types.h>
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup system_manager
/// \~english @par Summary:
///       Responds to SystemManager that debug dump output has finished
/// \~english @param [in] f_messageStr
///       std::string & - Dump data
/// \~english @retval
///       - None
/// \~english @par Preconditions:
///       - None
/// \~english @par Change of the internal state:
///       - No change of internal state by this API occurs
/// \~english @par Causes of failures:
///       - None
/// \~english @par Detail:
///       Send OnDebugDumpResponseReceived to SystemManager and request
///       to write the contents of dump data to /tmp/systemmanager_debugdump.log.\n
///       The maximum size of the dump buffer is 4KByte.\n
///       When dumping more than 4KBytes, the overflow from the buffer is discarded.
///
/// \~english @par Classification:
///       - Public
/// \~english @see
///
////////////////////////////////////////////////////////////////////////////////////////////

VOID SendDebugDumpResponseToSystemManager(std::string & f_messageStr);  // NOLINT (runtime/references)

#endif /*__SS_SYSTEM_MANAGER_CLIENT_IF_LOCAL_H__*/  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
