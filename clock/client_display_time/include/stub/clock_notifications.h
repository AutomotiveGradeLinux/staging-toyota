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
 * @file clock_notifications.h
 * @brief Notification names defined in the file.
 */
#ifndef CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_CLOCK_NOTIFICATIONS_H_
#define CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_CLOCK_NOTIFICATIONS_H_

#include <agl_thread.h>

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup vehicle_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup clock
 *  @ingroup vehicle_service
 *  @{
 */

/**
 * \~english clock service availability notify.
 */
#define NTFY_Clock_Availability MN_CLOCK"/Availability"

/** @}*/  // end of clock
/** @}*/  // end of vehicle_service
/** @}*/  // end of BaseSystem

#endif  // CLOCK_CLIENT_DISPLAY_TIME_INCLUDE_VEHICLE_SERVICE_CLOCK_NOTIFICATIONS_H_
