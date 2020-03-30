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
 *    CommUsbIf.h
 * @brief
 *    CommUSB service-to-service interface
 */
#ifndef POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_COMMUSBIF_H_
#define POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_COMMUSBIF_H_

/*---------------------------------------------------------------------------------*
 * Incluce                                                                         *
 *---------------------------------------------------------------------------------*/
#include <native_service/frameworkunified_types.h>
#include <native_service/frameworkunified_framework_if.h>
//#include <stub/commusb_api.h>
//#include <stub/commusb_notifications.h>

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
EFrameworkunifiedStatus CommUsbIfAttachCallbacksToDispatcher(    // NOLINT(readability/nolint)
                                                _FrameworkunifiedProtocolCallbackHandler const* p_msg_handler,
                                                unsigned int ui_handler_count);
EFrameworkunifiedStatus CommUsbIfDetachCallbacksFromDispatcher(const PUI_32 pui_cmd_array, UI_32 ui_command_count);
EFrameworkunifiedStatus CommUsbIfNotifyOnCommUSBAvailability(CbFuncPtr fp_on_cmd);
void CommUsbIfSetAvailability(BOOL b_is_available);
#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_SERVER_INCLUDE_SERVICEINTERFACE_COMMUSBIF_H_
