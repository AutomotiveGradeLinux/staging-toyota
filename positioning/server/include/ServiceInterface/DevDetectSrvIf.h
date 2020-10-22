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
 *    DevDetectSrvIf.h
 * @brief
 *    DevDetectSrv service-to-service interface
 */
#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DEVDETECTSRVIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DEVDETECTSRVIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <system_service/ss_devicedetection_service_ifc.h>
#include <system_service/ss_devicedetection_service_protocol.h>

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
void DevDetectSrvIfSetAvailability(BOOL b_is_available);
EFrameworkunifiedStatus DevDetectSrvIfInitialize(void);
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnDeviceDetectionAvailability(CbFuncPtr fp_call_back_fn);
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnOpenSessionAck(CbFuncPtr fp_call_back_fn, BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfNotifyOnCloseSessionAck(CbFuncPtr fp_call_back_fn, BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfOpenSessionRequest(BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfDecodeOpenSessionResponse(BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfRegisterForDeviceDetectionEvent(SS_DeviceDetectionServerEvents fe_dev_detect_event, \
              CbFuncPtr fp_call_back_fn, PCSTR p_file_path, BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfUnRegisterForDeviceDetectionEvent(SS_DeviceDetectionServerEvents fe_dev_detect_event, \
              BOOL* pb_is_available);
EFrameworkunifiedStatus DevDetectSrvIfCloseSessionRequest(BOOL* pb_is_available);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_DEVDETECTSRVIF_H_
