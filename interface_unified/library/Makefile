#
# @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# installed shared library(.so)
INST_SHLIBS := libSS_SystemIfUnified

# compiled sources
libSS_SystemIfUnified_SRCS := ss_framework_if.cpp \
                              ss_heartbeat_client.cpp \
                              ss_sm_client.cpp \
                              ss_string_maps.cpp \
                              ss_system_process.cpp \
                              ss_system_timer.cpp \
                              ss_last_to_order.cpp
libSS_SystemIfUnified_SRCS += ss_devicedetection_service_ifc.cpp
libSS_SystemIfUnified_SRCS += ss_logger_service_ifc.cpp
libSS_SystemIfUnified_SRCS += ss_power_client.cpp \
                              ss_pwrsvc_if.cpp

# additional source path
VPATH += ./src
VPATH += ./include/$(COMPONENT_NAME)

# installed headers
INST_HEADERS  = interface_unified.h
INST_HEADERS += interfaceunified_system_application.h \
                interfaceunified_system_types.h \
                ss_client_names.h \
                ss_devicedetection_service.h \
                ss_devicedetection_service_ifc.h \
                ss_devicedetection_service_notifications.h \
                ss_devicedetection_service_protocol.h \
                ss_devicedetection_service_types.h \
                ss_error_event_common.h \
                ss_error_message.h \
                ss_heartbeat_if.h \
                ss_heartbeat_notifications.h \
                ss_heartbeat_service_protocol.h \
                ss_logger_service.h \
                ss_logger_service_ifc.h \
                ss_logger_service_notifications.h \
                ss_logger_service_protocol.h \
                ss_power_service_if.h \
                ss_power_service_notifications.h \
                ss_power_service_protocol.h \
                ss_power_service.h \
                ss_services.h \
                ss_sm_client_if.h \
                ss_sys_boot_area_map.h \
                ss_sm_thread_names.h \
                ss_string_maps.h \
                ss_system_if.h \
                ss_system_manager_if.h \
                ss_system_manager_notifications.h \
                ss_system_manager_protocol.h \
                ss_system_process.h \
                ss_system_timer.h \
                ss_system_types.h \
                ss_boot_map.h \
                ss_templates.h \
                ss_test_clients.h \
                ss_version.h \
                ss_last_to_order.h
INST_HEADERS += ss_logger_store_logs.h
INST_HEADERS += ss_devicedetection_service_local.h \
                ss_devicedetection_service_protocol_local.h \
                ss_devicedetection_service_types_local.h \
                ss_error_event_common_local.h \
                ss_sm_client_if_local.h \
                ss_sm_thread_names_local.h \
                ss_system_manager_if_local.h \
                ss_system_manager_notifications_local.h \
                ss_system_manager_protocol_local.h \
                ss_logger_service_local.h \
                ss_power_service_local.h \
                ss_power_service_notifications_local.h



# additional include path
CPPFLAGS += -I./include/
CPPFLAGS += -I../../rom_access_library/library/include/

# additional compile option
CPPFLAGS += -DLINUX -fdata-sections -ffunction-sections
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3

CPPFLAGS_ss_devicedetection_service_ifc.os = -fno-exceptions
CPPFLAGS_ss_logger_service_ifc.os = -fno-exceptions
CPPFLAGS_ss_system_timer.os = -fno-exceptions
CPPFLAGS_ss_string_maps.os = -fno-exceptions
CPPFLAGS_ss_client_session.os = -fno-exceptions
CPPFLAGS_ss_framework_if.os = -fno-exceptions
CPPFLAGS_ss_ss_heartbeat_client.os = -fno-exceptions
CPPFLAGS_ss_parameter.os = -fno-exceptions
CPPFLAGS_ss_parameter_map.os = -fno-exceptions
CPPFLAGS_ss_str_stream.os = -fno-exceptions
CPPFLAGS_ss_system_config_reader.os = -fno-exceptions
CPPFLAGS_ss_system_shared_memory.os = -fno-exceptions
CPPFLAGS_ss_plm_if.os = -fno-exceptions
CPPFLAGS_ss_power_client.os = -fno-exceptions
CPPFLAGS_ss_pwrsvc_if.os = -fno-exceptions

LDFLAGS += -Wl,--no-as-needed

# linked libraries
LDLIBS += -Wl,-Bdynamic
LDLIBS += -Wl,-Bdynamic
LDLIBS += -Wl,-Bdynamic -lstdc++

LDFLAGS += -g -Wl,--gc-sections -shared


include ../../system_service.mk
