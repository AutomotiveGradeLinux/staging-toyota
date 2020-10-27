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
/// \ingroup  system_manager
/// \file ss_system_manager_notifications.h
/// \brief    This file supports the System Manager client interface.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef SS_SYSTEM_MANAGER_NOTIFICATIONS_H_  // NOLINT (build/header_guard)
#define SS_SYSTEM_MANAGER_NOTIFICATIONS_H_

#include "system_service/ss_services.h"

/// \addtogroup   system_manager
/// @{

#define NTFY_SSSystemMgrAvailability  SERVICE_SYSMANAGER"/Availability"  ///< System Manager availability -
                                                                 // eFrameworkunifiedServiceAvailable or eInterfaceunifiedServiceUnavailable
#define NTFY_SSSystemMgrState      SERVICE_SYSMANAGER"/State"   ///< System Manager state - Init,
                                                                 // Connecting, Disconnecting and Ready
#define NTFY_SSSystemMgrPowerOnOff    SERVICE_SYSMANAGER"/PowerOnOffState"    ///< Power ON notification for
                                                                              // Audio & Display
#define NTFY_SSSystemMgrStartUpType   SERVICE_SYSMANAGER"/StartUpType"           ///< Notification for start up type
#define NTFY_SSServiceWakeupStatus    SERVICE_SYSMANAGER"/ServiceWakeupStatus"   ///< Service Wakeup state
#define NTFY_SSNeedAplRestart      SERVICE_SYSMANAGER"/NeedAplRestart"        ///< Notification for Need APL Restart


/// Notification used by HMI to know when a Shutdown Request has been received
/// from the _CWORD56_
#define NTFY_SSSystemMgrShutdownStarted SERVICE_SYSMANAGER"/ShutdownStarted"

/// @}

#endif /* SS_SYSTEM_MANAGER_NOTIFICATIONS_H_ */  // NOLINT (build/header_guard)
