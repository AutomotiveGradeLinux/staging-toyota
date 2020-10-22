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
VPATH = ./src ./../../server/src
#######################################
INST_SHLIBS = libprimary

libprimary_SRCS = pri_main.c pri_api.c tskm_port_pf.c tskm_comm.c tskm_debug.c
LDFLAGS += -Wl,--no-as-needed
######### linked library (dynamic) #############
LDLIBS += -lpthread
LDLIBS += -lstdc++
LDLIBS += -lrt
LDLIBS += -lSS_SystemIfUnified
LDLIBS += -lNS_FrameworkUnified
LDLIBS += -lcommon

######### add library path #############


CPPFLAGS =  -I./include/ -I./../../include-share -I./../../server/include -I./../libtskmcfg/include

CPPFLAGS += -fno-exceptions

include ../../../system_service.mk
