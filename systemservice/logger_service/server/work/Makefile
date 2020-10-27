#
# @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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
INST_PROGS = SS_LoggerService


######### compiled sources  #############
SS_LoggerService_SRCS += mod_version.c
SS_LoggerService_SRCS += cached_file_writer.cpp
SS_LoggerService_SRCS += file_writer.cpp
SS_LoggerService_SRCS += mem_reader.cpp
SS_LoggerService_SRCS += loggerservice_application.cpp
SS_LoggerService_SRCS += loggerservice_main.cpp
SS_LoggerService_SRCS += loggerservicedebug_child_thread.cpp
SS_LoggerService_SRCS += loggerservicedebug_writer_Evntworker.cpp
SS_LoggerService_SRCS += queue_reader.cpp
SS_LoggerService_SRCS += reader.cpp
SS_LoggerService_SRCS += reader_writer.cpp
SS_LoggerService_SRCS += ss_logger_cfg.cpp
SS_LoggerService_SRCS += ss_logger_device_detection.cpp
SS_LoggerService_SRCS += ss_logger_error_event.cpp
SS_LoggerService_SRCS += ss_logger_error_event_archive.cpp
SS_LoggerService_SRCS += ss_logger_error_event_can_evt_reply.cpp
SS_LoggerService_SRCS += ss_logger_error_event_cfg.cpp
SS_LoggerService_SRCS += ss_logger_error_event_responses.cpp
SS_LoggerService_SRCS += ss_logger_error_event_storage.cpp
SS_LoggerService_SRCS += ss_logger_fs_directory.cpp
SS_LoggerService_SRCS += ss_logger_popups.cpp
SS_LoggerService_SRCS += ss_logger_reader_writer_control.cpp
SS_LoggerService_SRCS += ss_logger_server_callbacks.cpp
SS_LoggerService_SRCS += ss_logger_util.cpp
SS_LoggerService_SRCS += udp_writer.cpp
SS_LoggerService_SRCS += writer.cpp
SS_LoggerService_SRCS += udp_file_writer.cpp

######## add source path #############
VPATH += ../src
VPATH += ../include/$(COMPONENT_NAME)
VPATH += -I../include/readerWriter/reader
VPATH += -I../include/readerWriter/writer


######### add include path #############
CPPFLAGS += -I./
CPPFLAGS += -I../include
CPPFLAGS += -I../include/readerWriter
CPPFLAGS += -I../include/readerWriter/reader
CPPFLAGS += -I../include/readerWriter/writer
CPPFLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat
######### add compile option #############
CPPFLAGS += -DRELEASE_BUILD
CPPFLAGS += -DLINUX
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

CPPFLAGS_mem_reader.o = -fno-exceptions
CPPFLAGS_reader.o = -fno-exceptions
CPPFLAGS_reader_writer.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event_archive.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event_can_evt_reply.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event_cfg.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event_responses.o = -fno-exceptions
CPPFLAGS_ss_logger_error_event_storage.o = -fno-exceptions
CPPFLAGS_ss_logger_fs_directory.o = -fno-exceptions
CPPFLAGS_ss_logger_popups.o = -fno-exceptions
CPPFLAGS_ss_logger_reader_writer_control.o = -fno-exceptions
CPPFLAGS_ss_logger_server_callbacks.o = -fno-exceptions
CPPFLAGS_ss_logger_util.o = -fno-exceptions
CPPFLAGS_writer.o = -fno-exceptions
CPPFLAGS_mod_version.o = -fno-exceptions
CPPFLAGS_cashed_file_writer.o = -fno-exceptions
CPPFLAGS_file_writer.o = -fno-exceptions
CPPFLAGS_loggerservice_application.o = -fno-exceptions
CPPFLAGS_loggerservice_main.o = -fno-exceptions
CPPFLAGS_loggerservicedebug_child_thread.o = -fno-exceptions
CPPFLAGS_loggerservicedebug_writer_Evntworker.o = -fno-exceptions
CPPFLAGS_queue_reader.o = -fno-exceptions
CPPFLAGS_ss_logger_device_detection.o = -fno-exceptions
CPPFLAGS_udp_file_writer.o = -fno-exceptions
CPPFLAGS_udp_writer.o = -fno-exceptions

######### linked library #############
LDFLAGS += -L../lib/
LDLIBS += -Wl,-Bstatic -lSS_LoggerStoreLogs
LDLIBS += -Wl,-Bstatic -lPFDRECThread

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lc #Substitute for libsocket
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lrt # for mq_xxx
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -ltar
LDLIBS += -Wl,-Bdynamic -lz
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy
LDLIBS += -Wl,-Bdynamic -lns_backup #for PFDRECTread
LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lcrypto
LDLIBS += -Wl,-Bdynamic -lextension
LDLIBS += -Wl,-Bdynamic -lClock_API


LINK_CXX=Y
include ../../../system_service.mk
