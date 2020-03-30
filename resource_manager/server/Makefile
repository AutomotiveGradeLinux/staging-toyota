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
INST_PROGS = resm

######### compiled sources  #############
resm_SRCS += resm.cpp
resm_SRCS += proc_watch.cpp
resm_SRCS += resourcemanager_application.cpp
resm_SRCS += cpuload_custom.cpp
resm_SRCS += resmgr_srvr_stub.cpp

######### add source path #############
VPATH += ./src
VPATH += ./include/$(COMPONENT_NAME)
VPATH += ../client

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I../client

######## add compile option ########
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGAPPZONES=104,64,31,30,29,28
CPPFLAGS += -DTARGET_SOC_$(TARGET_SOC)
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND
CPPFLAGS += -fno-exceptions

######### linked library (dynamic) #############
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
#LDLIBS += -Wl,-Bdynamic -lsoc_temperature_hal
LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -lrpc

LDLIBS += -Wl,-Bdynamic -lev
#LDLIBS += -Wl,-Bdynamic -lDiagCodeAPI

include ../../system_service.mk
