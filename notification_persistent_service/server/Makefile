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
INST_PROGS = NS_NPPService

######### add source path #############
VPATH += ./src
VPATH += ./include/$(COMPONENT_NAME)

######### installed library(*.a) #############
#INST_LIBS =

######### installed shared library(*.so) #############
#INST_SHLIBS =

######### install headers(*.h)  #############
INST_HEADERS = notification_persistent_service.h

######### compiled sources  #############
NS_NPPService_SRCS = app_states.cpp \
		     ns_npp.cpp \
		     ns_npp_archive.cpp \
		     ns_npp_binary_accesser.cpp \
		     ns_npp_copy_worker.cpp \
		     ns_npp_fs_directory.cpp \
		     ns_npp_handlelist.cpp \
		     ns_npp_nor_persistence_worker_thread.cpp \
		     ns_npp_notification.cpp \
		     ns_npp_notification_manager.cpp \
		     ns_npp_notification_receiver.cpp \
		     ns_npp_persist_file.cpp \
		     ns_npp_persist_folder.cpp \
		     ns_npp_persistence.cpp \
		     ns_npp_persistence_manager.cpp \
		     ns_npp_persistent_data.cpp \
		     ns_npp_personalization_manager.cpp \
		     ns_npp_registry_entry.cpp \
		     ns_npp_regular_notification.cpp \
		     ns_npp_state_nor_persistence_notification.cpp \
		     ns_npp_state_notification.cpp \
		     ns_npp_state_persistence_notification.cpp \
		     ns_npp_state_persistence_user_notification.cpp \
		     notificationpersistentservice_application.cpp \
		     notificationpersistentservice_main.cpp

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat
######### add compile option #############
ENABLE_PARA_INST += install-prog # PARALLEL MAKE
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

######### linked library #############

######### linked library (dynamic) #############
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -ltar
LDLIBS += -Wl,-Bdynamic -lz
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy #-lPosixBasedOS001ClockCycleApi -lPosixBasedOS001MsgApi -lPosixBasedOS001GetPrioApi -lPosixBasedOS001TimeApi -lPosixBasedOS001SlogApi

LINK_CXX=Y

######### add library path #############

include ../../native_service.mk
