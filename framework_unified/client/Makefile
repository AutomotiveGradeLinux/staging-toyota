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

######### installed program #############
#INST_PROGS =

######### installed library(*.a) #############
#INST_LIBS =

######### installed shared library(*.so) #############
INST_SHLIBS = libNS_FrameworkUnified

######### install headers(*.h)  #############

VPATH += ./include/$(COMPONENT_NAME)
# native
INST_HEADERS  = ns_config_parser_if.h
INST_HEADERS += ns_reader.h
INST_HEADERS += ns_writer.h
INST_HEADERS += ns_eventlogger.h
INST_HEADERS += ns_logger_if.h
INST_HEADERS += ns_message_center_if.h
INST_HEADERS += ns_np_service.h
INST_HEADERS += ns_np_service_if.h
INST_HEADERS += ns_np_service_nor_persistence.h
INST_HEADERS += ns_np_service_notification.h
INST_HEADERS += ns_np_service_protocol.h
INST_HEADERS += ns_np_service_nor_persistence_internal.h
INST_HEADERS += ns_np_types.h
INST_HEADERS += ns_plogger_if.h
INST_HEADERS += ns_shared_mem.h
INST_HEADERS += ns_shared_mem_if.h
INST_HEADERS += ns_sharedmem.h
INST_HEADERS += ns_sharedmem_reader.h
INST_HEADERS += ns_sharedmem_writer.h
INST_HEADERS += ns_ringbuffer.h
INST_HEADERS += ns_system_mode.h
INST_HEADERS += ns_timer_if.h
INST_HEADERS += ns_timer_if.hpp
INST_HEADERS += ns_transmit_log.h
INST_HEADERS += ns_utility.hpp
INST_HEADERS += ns_utility_if.h
INST_HEADERS += ns_utility_sys.hpp
INST_HEADERS += ns_version_if.h
INST_HEADERS += ns_resource_controler.h
INST_HEADERS += XMLParser.h
INST_HEADERS += ns_msgs.h
INST_HEADERS += ns_mc_system_info.h

# inc
INST_HEADERS += ns_version.h
INST_HEADERS += frameworkunified_service_protocol.h
INST_HEADERS += frameworkunified_types.h
INST_HEADERS += wpf_legacy.h
INST_HEADERS += framework_unified.h

# nsrcs
INST_HEADERS += ns_rcs_data_handler.hpp
INST_HEADERS += ns_rcs_logger_types.h
INST_HEADERS += ns_rcs_plugin.hpp

# _CWORD77_
INST_HEADERS += ns__CWORD77__common.h
INST_HEADERS += ns__CWORD77__data_pool_if.h
INST_HEADERS += ns__CWORD77__data_pool_key.h
INST_HEADERS += ns__CWORD77__data_pool_table.h
INST_HEADERS += ns__CWORD77__notifications.h
INST_HEADERS += ns__CWORD77__types.h
INST_HEADERS += frameworkunified__CWORD77__service_if.h
INST_HEADERS += frameworkunified__CWORD77__session_if.h

# utility
INST_HEADERS += ns_endianess.h
INST_HEADERS += nslogutil_cmd_if.h
INST_HEADERS += nsfw_message.h
INST_HEADERS += ns_util_directory.h
INST_HEADERS += ns_util_crc.h

# framework
INST_HEADERS += frameworkunified_application.h
INST_HEADERS += frameworkunified_dispatcher.h
INST_HEADERS += frameworkunified_framework_error.hpp
INST_HEADERS += frameworkunified_framework_if.h
INST_HEADERS += frameworkunified_framework_sync.h
INST_HEADERS += frameworkunified_framework_system_if.h
INST_HEADERS += frameworkunified_framework_types.h
INST_HEADERS += frameworkunified_multithreading.h
INST_HEADERS += frameworkunified_service_if.h
INST_HEADERS += frameworkunified_thread_priority.h
INST_HEADERS += frameworkunified_timer.h

# framework/statemachine
INST_HEADERS += frameworkunified_sm_action.h
INST_HEADERS += frameworkunified_sm_compositestate.h
INST_HEADERS += frameworkunified_sm_conditionconnector.h
INST_HEADERS += frameworkunified_sm_deephistorystate.h
INST_HEADERS += frameworkunified_sm_dispatcher.h
INST_HEADERS += frameworkunified_sm_eventdata.h
INST_HEADERS += frameworkunified_sm_eventfactory.h
INST_HEADERS += frameworkunified_sm_externaltransition.h
INST_HEADERS += frameworkunified_sm_framework_dispatch.h
INST_HEADERS += frameworkunified_sm_framework_if.h
INST_HEADERS += frameworkunified_sm_framework_types.h
INST_HEADERS += frameworkunified_sm_guard.h
INST_HEADERS += frameworkunified_sm_historystate.h
INST_HEADERS += frameworkunified_sm_hsm.h
INST_HEADERS += frameworkunified_sm_hsmframework.h
INST_HEADERS += frameworkunified_sm_internaltransition.h
INST_HEADERS += frameworkunified_sm_leafstate.h
INST_HEADERS += frameworkunified_sm_localtransition.h
INST_HEADERS += frameworkunified_sm_multithreading.h
INST_HEADERS += frameworkunified_sm_orthogonalstate.h
INST_HEADERS += frameworkunified_sm_reaction.h
INST_HEADERS += frameworkunified_sm_shallowhistorystate.h
INST_HEADERS += frameworkunified_sm_state.h
INST_HEADERS += frameworkunified_sm_transition.h

# xmlparser
INST_HEADERS += ns_xml_reader.h
INST_HEADERS += ns_xml_writer.h
INST_HEADERS += ns_xmlparser_if.h

######### compiled sources  #############

# NS_Logger
libNS_FrameworkUnified_SRCS += ./NS_Logger/src/ns_logger.cpp
CPPFLAGS += -I./NS_Logger/include

# NS_MessageQueue
libNS_FrameworkUnified_SRCS += ./NS_MessageQueue/src/ns_msg_queue.c
libNS_FrameworkUnified_SRCS += ./NS_MessageQueue/src/ns_mq_string.c
CPPFLAGS += -I./NS_MessageQueue/include

# NS_SharedMemIf
libNS_FrameworkUnified_SRCS += ./NS_SharedMemIf/src/ns_shared_mem_if.cpp
libNS_FrameworkUnified_SRCS += ./NS_SharedMemIf/src/ns_sharedmem.cpp
libNS_FrameworkUnified_SRCS += ./NS_SharedMemIf/src/ns_sharedmem_reader.cpp
libNS_FrameworkUnified_SRCS += ./NS_SharedMemIf/src/ns_sharedmem_writer.cpp

# NS_RingBuffer
libNS_FrameworkUnified_SRCS += ./NS_RingBuffer/src/ns_ringbuffer.cpp

# NS_UtilityCenter
libNS_FrameworkUnified_SRCS += ./NS_UtilityCenter/src/ns_endianess.cpp
libNS_FrameworkUnified_SRCS += ./NS_UtilityCenter/src/ns_util_crc.cpp
libNS_FrameworkUnified_SRCS += ./NS_UtilityCenter/src/ns_util_directory.cpp
libNS_FrameworkUnified_SRCS += ./NS_UtilityCenter/src/ns_utility.cpp
libNS_FrameworkUnified_SRCS += ./NS_UtilityCenter/src/frameworkunified_version.cpp
CPPFLAGS += -I./NS_UtilityCenter/include

# NS_MessageCenter
libNS_FrameworkUnified_SRCS += ./NS_MessageCenter/src/ns_message_center.c
CPPFLAGS += -I./NS_MessageCenter/include

# NS_ConfigParser
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_cfg_parser.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_cfg_reader.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_cfg_writer.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_config_reader.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_config_reader_writer.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_config_writer.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_reader.cpp
libNS_FrameworkUnified_SRCS += ./NS_ConfigParser/src/ns_writer.cpp
CPPFLAGS += -I./NS_ConfigParser/include

# NS_NPServiceIf
libNS_FrameworkUnified_SRCS += ./NS_NPServiceIf/src/ns_np_service_api.c
libNS_FrameworkUnified_SRCS += ./NS_NPServiceIf/src/ns_np_service_nor_persistence.cpp
CPPFLAGS += -I./NS_NPServiceIf/include

# NS_Timer
libNS_FrameworkUnified_SRCS += ./NS_Timer/src/mod_version.c
libNS_FrameworkUnified_SRCS += ./NS_Timer/src/ns_timer.c
libNS_FrameworkUnified_SRCS += ./NS_Timer/src/ns_timer_class.cpp
CPPFLAGS += -I./NS_Timer/include

# NS_FrameworkCore
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_dispatcher.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_dispatch.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_error.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_logger.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_message_center.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_npservice.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_session.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_sync.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_framework_system.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_msgprofiler.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_multithreading.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_service_if.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_thread_priority.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/frameworkunified_timer.cpp
CPPFLAGS += -I./NS_FrameworkCore/include

# NS_FrameworkCore/src/statemachine
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_action.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_compositestate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_conditionconnector.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_deephistorystate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_dispatcher.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_externaltransition.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_finalstate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_framework_dispatch.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_guard.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_historystate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_hsm.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_hsmframework.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_leafstate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_localtransition.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_multithreading.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_orthogonalstate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_shallowhistorystate.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_state.cpp
libNS_FrameworkUnified_SRCS += ./NS_FrameworkCore/src/statemachine/frameworkunified_sm_transition.cpp
CPPFLAGS += -I./NS_FrameworkCore/include/statemachine

# NS__CWORD77__ServiceIf
libNS_FrameworkUnified_SRCS += ./NS__CWORD77__ServiceIf/src/ns__CWORD77__data_pool_table.cpp
libNS_FrameworkUnified_SRCS += ./NS__CWORD77__ServiceIf/src/frameworkunified__CWORD77__service_if.cpp
libNS_FrameworkUnified_SRCS += ./NS__CWORD77__ServiceIf/src/frameworkunified__CWORD77__session_if.cpp

# NS_ResourceControler
libNS_FrameworkUnified_SRCS += ./NS_ResourceControler/src/ns_resource_controler.cpp

######### add include path #############
CPPFLAGS += -I./include

######### add compile option #############

LDFLAGS += -Wl,--no-as-needed


######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lrt
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -lcommon
LINK_SHLIB_CXX = Y

# buildversioncheck
INST_HEADERS += ns_buildversioncheck.hpp


SUBDIRS := NS_XMLConfigeParser NS_Utility



all:

install-data:install_cfg

install_cfg:
	install -d -m 775 $(DESTDIR)/usr/agl/conf/ns_logger
	install -m 644 -t $(DESTDIR)/usr/agl/conf/ns_logger NativeServices/scfg/ns_logger_frameworkunifiedlog__CWORD84_.cfg

include ../../native_service.mk
