//
// @copyright Copyright (c) 2017-2019 TOYOTA MOTOR CORPORATION.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
/**
 * @file interface_unified.h
 * @brief \~english include all interface_unified head files
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
// include guard
#ifndef SYSTEMSERVICE_INTERFACEUNIFIED_H_  // NOLINT(build/header_guard)
#define SYSTEMSERVICE_INTERFACEUNIFIED_H_

#include "system_service/interfaceunified_system_application.h"
#include "system_service/interfaceunified_system_types.h"
#include "system_service/ss_boot_map.h"
#include "system_service/ss_client_names.h"
#include "system_service/ss_devicedetection_service.h"
#include "system_service/ss_devicedetection_service_ifc.h"
#include "system_service/ss_devicedetection_service_notifications.h"
#include "system_service/ss_devicedetection_service_protocol.h"
#include "system_service/ss_devicedetection_service_types.h"
#include "system_service/ss_error_event_common.h"
#include "system_service/ss_error_message.h"
#include "system_service/ss_heartbeat_if.h"
#include "system_service/ss_heartbeat_notifications.h"
#include "system_service/ss_heartbeat_service_protocol.h"
#include "system_service/ss_last_to_order.h"
#include "system_service/ss_logger_service.h"
#include "system_service/ss_logger_service_ifc.h"
#include "system_service/ss_logger_service_notifications.h"
#include "system_service/ss_logger_service_protocol.h"
#include "system_service/ss_services.h"
#include "system_service/ss_sm_client_if.h"
#include "system_service/ss_sm_thread_names.h"
#include "system_service/ss_string_maps.h"
#include "system_service/ss_sys_boot_area_map.h"
#include "system_service/ss_system_if.h"
#include "system_service/ss_system_manager_if.h"
#include "system_service/ss_system_manager_notifications.h"
#include "system_service/ss_system_manager_protocol.h"
#include "system_service/ss_system_process.h"
#include "system_service/ss_system_timer.h"
#include "system_service/ss_system_types.h"
#include "system_service/ss_templates.h"
#include "system_service/ss_test_clients.h"
#include "system_service/ss_version.h"

// reference from logger_store_logs
#include "system_service/ss_logger_store_logs.h"

// files with local
// reference from device_detection_service
#include "system_service/ss_devicedetection_service_local.h"
#include "system_service/ss_devicedetection_service_protocol_local.h"
#include "system_service/ss_devicedetection_service_types_local.h"
// reference from system_manager
#include "system_service/ss_error_event_common_local.h"
#include "system_service/ss_sm_client_if_local.h"
#include "system_service/ss_sm_thread_names_local.h"
#include "system_service/ss_system_manager_if_local.h"
#include "system_service/ss_system_manager_notifications_local.h"

#include "system_service/ss_system_manager_protocol_local.h"
// reference from logger_service
#include "system_service/ss_logger_service_local.h"

// reference from power_service
#include "system_service/ss_power_service_if.h"
#include "system_service/ss_power_service_notifications.h"
#include "system_service/ss_power_service_protocol.h"
#include "system_service/ss_power_service.h"
#include "system_service/ss_power_service_local.h"
#include "system_service/ss_power_service_notifications_local.h"

#endif  // SYSTEMSERVICE_INTERFACEUNIFIED_H_
/** @}*/
/** @}*/
/** @}*/
