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
 * @file ps_psmshadow_notifications.h
 */

#ifndef PS_PSM_SHADOW_NOTIFICATIONS_H_  // NOLINT(build/header_guard)

#include "agl_thread.h"

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup peripheralservice
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup PS_PSMShadow
 *  @ingroup peripheralservice
 *  @{
 */

/**
 * \~english PS_PSMShadow service availability notify.
 */
#define NTFY_PSMShadowService_Availability   MN_PS_PSMSHADOW"/Availability"

/**
 * \~english init complete notify.
 */
#define NTFY_PSM_INITCOMP                    MN_PS_PSMSHADOW"/InitComp"

/**
 * \~english init complete2 notify.
 */
#define NTFY_PSM_INITCOMP2                   MN_PS_PSMSHADOW"/InitComp2"

// to be delete code.
// Voltage, temperature and fan data to DIAG / HMI.
#define NTFY_PSM_VTG_TEMP_FAN_DATA           MN_PS_PSMSHADOW"/VolTempFanInfo"
// Audio High Temp notification to Audio service.
#define NTFY_PSM_AUDIO_HIGH_TEMP             MN_PS_PSMSHADOW"/AudioHighTemp"

/** @}*/  // end of PS_PSMShadow
/** @}*/  // end of peripheralservice
/** @}*/  // end of BaseSystem

#endif  // PS_PSM_SHADOW_NOTIFICATIONS_H_
