/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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
 * @file framework_unified.h
 * @brief \~english  include all framework_unified head files
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup framework
 *  @ingroup framework_unified
 *  @{
 */
#ifndef NATIVESERVICE_FRAMEWORKUNIFIED_H_  // NOLINT  (build/header_guard)
#define NATIVESERVICE_FRAMEWORKUNIFIED_H_

#include <native_service/ns_config_parser_if.h>
#include <native_service/ns_eventlogger.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_message_center_if.h>
#include <native_service/ns_np_service.h>
#include <native_service/ns_np_service_if.h>
#include <native_service/ns_np_service_nor_persistence.h>
#include <native_service/ns_np_service_notification.h>
#include <native_service/ns_np_service_protocol.h>
#include <native_service/ns_plogger_if.h>
#include <native_service/ns_shared_mem.h>
#include <native_service/ns_shared_mem_if.h>
#include <native_service/ns_sharedmem.h>
#include <native_service/ns_sharedmem_reader.h>
#include <native_service/ns_sharedmem_writer.h>
#include <native_service/ns_ringbuffer.h>
#include <native_service/ns_system_mode.h>
#include <native_service/ns_timer_if.h>
#include <native_service/ns_timer_if.hpp>
#include <native_service/ns_transmit_log.h>
#include <native_service/ns_utility.hpp>
#include <native_service/ns_utility_if.h>
#include <native_service/ns_utility_sys.hpp>
#include <native_service/ns_util_directory.h>
#include <native_service/ns_util_crc.h>
#include <native_service/ns_version_if.h>
#include <native_service/ns_resource_controler.h>
#include <native_service/ns_version.h>
#include <native_service/frameworkunified_service_protocol.h>
#include <native_service/frameworkunified_types.h>
#include <native_service/wpf_legacy.h>
#include <native_service/ns_rcs_data_handler.hpp>
#include <native_service/ns_rcs_logger_types.h>
#include <native_service/ns_rcs_plugin.hpp>
#include <native_service/ns__CWORD77__common.h>
#include <native_service/ns__CWORD77__data_pool_if.h>
#include <native_service/ns__CWORD77__data_pool_key.h>
#include <native_service/ns__CWORD77__data_pool_table.h>
#include <native_service/ns__CWORD77__notifications.h>
#include <native_service/ns__CWORD77__types.h>
#include <native_service/frameworkunified__CWORD77__service_if.h>
#include <native_service/frameworkunified__CWORD77__session_if.h>
#include <native_service/ns_buildversioncheck.hpp>
#include <native_service/ns_endianess.h>
#include <native_service/nslogutil_cmd_if.h>
#include <native_service/nsfw_message.h>
#include <native_service/frameworkunified_application.h>
#include <native_service/frameworkunified_dispatcher.h>
#include <native_service/frameworkunified_framework_if.h>
#include <native_service/frameworkunified_framework_sync.h>
#include <native_service/frameworkunified_framework_system_if.h>
#include <native_service/frameworkunified_framework_types.h>
#include <native_service/frameworkunified_multithreading.h>
#include <native_service/frameworkunified_service_if.h>
#include <native_service/frameworkunified_thread_priority.h>
#include <native_service/frameworkunified_timer.h>
#include <native_service/frameworkunified_sm_action.h>
#include <native_service/frameworkunified_sm_compositestate.h>
#include <native_service/frameworkunified_sm_conditionconnector.h>
#include <native_service/frameworkunified_sm_deephistorystate.h>
#include <native_service/frameworkunified_sm_dispatcher.h>
#include <native_service/frameworkunified_sm_eventdata.h>
#include <native_service/frameworkunified_sm_eventfactory.h>
#include <native_service/frameworkunified_sm_externaltransition.h>
#include <native_service/frameworkunified_sm_framework_dispatch.h>
#include <native_service/frameworkunified_sm_framework_if.h>
#include <native_service/frameworkunified_sm_framework_types.h>
#include <native_service/frameworkunified_sm_guard.h>
#include <native_service/frameworkunified_sm_historystate.h>
#include <native_service/frameworkunified_sm_hsm.h>
#include <native_service/frameworkunified_sm_hsmframework.h>
#include <native_service/frameworkunified_sm_internaltransition.h>
#include <native_service/frameworkunified_sm_leafstate.h>
#include <native_service/frameworkunified_sm_localtransition.h>
#include <native_service/frameworkunified_sm_multithreading.h>
#include <native_service/frameworkunified_sm_orthogonalstate.h>
#include <native_service/frameworkunified_sm_reaction.h>
#include <native_service/frameworkunified_sm_shallowhistorystate.h>
#include <native_service/frameworkunified_sm_state.h>
#include <native_service/frameworkunified_sm_transition.h>
#include <native_service/XMLParser.h>
#include <native_service/ns_xml_reader.h>
#include <native_service/ns_xml_wrter.h>
#include <native_service/ns_xmlparser_if.h>
#include <native_service/ns_reader.h>
#include <native_service/ns_writer.h>

#endif // NATIVESERVICE_FRAMEWORKUNIFIED_H_  NOLINT  (build/header_guard)
/**@}*/  // end of inc
/**@}*/  // end of framework_unified
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem
