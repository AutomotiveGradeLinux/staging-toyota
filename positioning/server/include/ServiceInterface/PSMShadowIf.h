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
 * @file
 *    PSMShadow.h
 * @brief
 *    PSMShadow service-to-service interface
 */
#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_PSMSHADOWIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_PSMSHADOWIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
#include <vehicle_service/positioning_base_library.h>
#include "ps_psmshadow_notifications.h"

/*---------------------------------------------------------------------------------*
 * Definition                                                                      *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Typedef declaration                                                             *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Struct declaration                                                              *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 * Prototype Declaration                                                           *
 *---------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
EFrameworkunifiedStatus PSMShadowIfNotifyOnPSMShadowAvailability(CbFuncPtr fp_on_cmd);
EFrameworkunifiedStatus PSMShadowIfNotifyOnPSMShadowInitComp(CbFuncPtr fp_on_cmd);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_PSMSHADOWIF_H_
