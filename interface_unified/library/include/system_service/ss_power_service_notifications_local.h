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
 * @file ss_power_service_notifications_local.h
 * @brief \~english This file supports the Power Service interface.
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup system_service
 *  @ingroup system_service
 *  @{
 */
#ifndef SS_POWER_SERVICE_NOTIFICATIONS_LOCAL_H_  // NOLINT (build/header_guard)
#define SS_POWER_SERVICE_NOTIFICATIONS_LOCAL_H_

#include "system_service/ss_services.h"


/// \addtogroup system_manager
/// @{

/// Depreciated !!
#define szNTFY_PowerLVI1        SERVICE_POWER"/LVI1"
/// Depreciated !!
#define szNTFY_PowerLVI2        SERVICE_POWER"/LVI2"
/// Indicates the current power level state
#define szNTFY_PowerLevel        SERVICE_POWER"/Level"

/// Notification that Power Service publishes for subscribers ( ie, HMI ) to
/// display a popup indicating the type of shutdown condition.
/// 1. Power Save
/// 2. BatteryCut
/// 3. Low Voltage
#define szNTFY_ShutdownPopup        SERVICE_POWER"/ShutdownPopup"

/// Notification that Power Service publishes for subscribers ( ie, HMI ) to
/// display a popup indicating the type of CPMShowPowerPopup.
/// 1. Normal
/// 2. Critical
/// 3. Application Critical
#define szNTFY_PowerPopup            SERVICE_POWER"/PowerPopup"

/// @}

#endif /* SS_POWER_SERVICE_NOTIFICATIONS_LOCAL_H_ */  // NOLINT (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
