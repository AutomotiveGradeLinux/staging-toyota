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
INST_PROGS = realtimeUsbLog

######### installed library(*.a) #############


######### installed shared library(*.so) #############


######### install headers(*.h)  #############


######### compiled sources  #############
realtimeUsbLog_SRCS += ss_logger_realtime_usb.cpp
realtimeUsbLog_SRCS += loggerservice_main.cpp
######### add source path #############
VPATH += ../src

######### add include path #############
CPPFLAGS += -I./

#CPPFLAGS += -I../inc
#CPPFLAGS += -I../../inc
CPPFLAGS += -I../../include
CPPFLAGS += -I../../../../system_manager/server/include
CPPFLAGS += -I../../../../system_manager/server/include/processlauncher


######### add compile option #############
CPPFLAGS += -DLINUX
CXXFLAGS += -std=c++11
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30
######### linked library #############


######### linked library (dynamic) #############
#LDLIBS += -Wl,-Bdynamic -lxxx
LDLIBS += -Wl,-Bdynamic -lcommon
#LDLIBS += -Wl,-Bdynamic -lextension
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy

LDLIBS += -Wl,-Bdynamic -lsssmcfg
#LDLIBS += -Wl,-Bdynamic -lMounter
LDLIBS += -Wl,-Bdynamic -lresm
LDLIBS += -Wl,-Bdynamic -lssver
LDLIBS += -Wl,-Bdynamic -lSS_RomAccessIf
#LDLIBS += -Wl,-Bdynamic -lDiagCodeAPI
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -ltar -lz
LDLIBS += -Wl,-Bdynamic -lrt
LDLIBS += -Wl,-Bdynamic -lvp

######### add library path #############

LDFLAGS += -g -Wl,--gc-sections

LINK_CXX=Y
include ../../../../system_service.mk
