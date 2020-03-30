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
INST_PROGS = SS_PowerService


######### compiled sources  #############
SS_PowerService_SRCS += ./src/powerservice_application.cpp
SS_PowerService_SRCS += ./src/powerservice_main.cpp
SS_PowerService_SRCS += ./src/ss_power.cpp
SS_PowerService_SRCS += ./src/ss_power_config.cpp
SS_PowerService_SRCS += ./src/ss_power_session.cpp
SS_PowerService_SRCS += ./src/ss_power_state_machine.cpp
SS_PowerService_SRCS += ./src/ss_pwr_test_client_handlers.cpp
SS_PowerService_SRCS += ./src/ss_supervisor_handlers.cpp
SS_PowerService_SRCS += ./src/ss_system_handlers.cpp


######### add include path #############
CPPFLAGS += -I./include


######### add compile option #############
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x09 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,13,9,8,3  ## open log-tag
CPPFLAGS += -DTARGET_SOC_$(TARGET_SOC)
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy

######### add library path #############
LDFLAGS += -g -Wl,--gc-sections

include ../../system_service.mk
