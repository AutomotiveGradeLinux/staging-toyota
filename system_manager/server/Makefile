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
INST_PROGS = SS_SystemManager

######### installed library(*.a) #############


######### installed shared library(*.so) #############


######### install headers(*.h)  #############


######### compiled sources  #############
SS_SystemManager_SRCS += ./src/processlauncher/ProcessLauncher.cpp
SS_SystemManager_SRCS += ./src/processlauncher/ProcessLauncher_if.cpp
SS_SystemManager_SRCS += ./src/heartbeat/ss_hb_if.cpp
SS_SystemManager_SRCS += ./src/heartbeat/ss_hb_service_protocol.cpp
SS_SystemManager_SRCS += ./src/heartbeat/ss_hb_session.cpp
SS_SystemManager_SRCS += ./src/heartbeat/ss_hb_thread.cpp
SS_SystemManager_SRCS += ./src/systemmanager_application.cpp
SS_SystemManager_SRCS += ./src/systemmanager_main.cpp
SS_SystemManager_SRCS += ./src/ss_sm_config.cpp
SS_SystemManager_SRCS += ./src/ss_sm_dynamic_launcher.cpp
SS_SystemManager_SRCS += ./src/ss_sm_version.cpp
SS_SystemManager_SRCS += ./src/ss_system_manager.cpp
SS_SystemManager_SRCS += ./src/ss_system_manager_callbacks.cpp
SS_SystemManager_SRCS += ./src/ss_system_manager_error_event.cpp
SS_SystemManager_SRCS += ./src/ss_system_manager_error_event_responses.cpp
SS_SystemManager_SRCS += ./src/ss_system_manager_error_event_triggers.cpp
SS_SystemManager_SRCS += ./src/ss_system_memory_monitor.cpp

######### add source path #############


######### add include path #############
CPPFLAGS += -I./
CPPFLAGS += -I./include
CPPFLAGS += -I./include/heartbeat
CPPFLAGS += -I./include/processlauncher
CPPFLAGS += -I./../../interface_unified/library/include

######### add compile option #############
CPPFLAGS += -DLINUX
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

CPPFLAGS_ProcessLauncher.o = -fno-exceptions
CPPFLAGS_ProcessLauncher_if.o = -fno-exceptions
CPPFLAGS_ss_hb_if.o = -fno-exceptions
CPPFLAGS_ss_hb_thread.o = -fno-exceptions
CPPFLAGS_systemmanager_application.o = -fno-exceptions
CPPFLAGS_systemmanager_main.o = -fno-exceptions
CPPFLAGS_ss_system_memory_monitor.o = -fno-exceptions
CPPFLAGS_ss_hb_service_protocol.o = -fno-exceptions
CPPFLAGS_ss_hb_session.o = -fno-exceptions
CPPFLAGS_ss_sm_dynamic_launcher.o = -fno-exceptions
CPPFLAGS_ss_sm_version.o = -fno-exceptions
CPPFLAGS_ss_system_manager.o = -fno-exceptions
CPPFLAGS_ss_system_manager_error_event.o = -fno-exceptions

######### linked library #############
#LDLIBS += -Wl,-Bstatic -lxxx
LDLIBS += -Wl,-Bstatic -lSS_LoggerStoreLogs

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lrt # for mq_xxx
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lssver

LDLIBS += -Wl,-Bdynamic -lresm
LDLIBS += -Wl,-Bdynamic -lsssmcfg
LDLIBS += -Wl,-Bdynamic -lDiagCodeAPI
LDLIBS += -Wl,-Bdynamic -lClock_API
LDLIBS += -Wl,-Bdynamic -lpower_hal

LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -ltar -lz
LDLIBS += -Wl,-Bdynamic -lrpc -lev

######### add library path #############
LDFLAGS += -g -Wl,--gc-sections
LDFLAGS += -Wl,--warn-unresolved-symbols
LDFLAGS += -Wl,--no-as-needed

LINK_CXX=Y

install-data: install_data

install_data:
	cpp -P -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_thread.h ./sm_launch__CWORD84_.xml ./sm_launch.tmp.xml
	$(SDKTARGETSYSROOT)/usr/agl/share/launch_xml2cfg.sh ./sm_launch.tmp.xml > ./sm_launch.cfg
	install -d -m 755 $(DESTDIR)/usr/agl/conf/BS/ss/system_manager/rwdata
	install -m 644 ./sm_launch.cfg $(DESTDIR)/usr/agl/conf/BS/ss/system_manager/rwdata
	install -d -m 755 $(DESTDIR)/nv/BS/ss/system_manager/rwdata
	install -d -m 755 $(DESTDIR)/usr/agl/share/BS/ss/system_manager/rodata
	install -m 644 ./version.txt $(DESTDIR)/usr/agl/share/BS/ss/system_manager/rodata
	install -m 644 ./PhaseInfo.txt $(DESTDIR)/usr/agl/share/BS/ss/system_manager/rodata

include ../../system_service.mk
