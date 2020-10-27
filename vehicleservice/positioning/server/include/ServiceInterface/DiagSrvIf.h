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
 * @file
 *    DiagSrvIf.h
 * @brief
 *    DiagSrv service-to-service interface
 */
#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DIAGSRVIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DIAGSRVIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <vehicle_service/positioning_base_library.h>

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
void DiagSrvIfSetRegistrationPermission(BOOL b_is_true);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DIAGSRVIF_H_
