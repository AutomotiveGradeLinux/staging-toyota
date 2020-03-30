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
 * @file ss_system_manager_notifications_local.h
 * @brief \~english This file supports the System Manager client interface.
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
#ifndef SS_SYSTEM_MANAGER_NOTIFICATIONS_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_MANAGER_NOTIFICATIONS_LOCAL_H_


/// TRUE  : UserMode is UserOn
/// FALSE : UserMode is UserOff
/// Notification for User Mode
#define NTFY_SSSystemMgrUserMode       SERVICE_SYSMANAGER"/UserMode"

/// Notification for data reset
#define NTFY_SSSystemMgrDataReset       SERVICE_SYSMANAGER"/DataReset"

/// Notification used by Audio and System Services for Dynamic Launch
#define NTFY_SSLastSourceType           SERVICE_SYSMANAGER"/LastSourceType"


#endif /* SS_SYSTEM_MANAGER_NOTIFICATIONS_LOCAL_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
