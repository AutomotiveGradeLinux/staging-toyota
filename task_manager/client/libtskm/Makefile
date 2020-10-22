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

VPATH = ./ ./src 

INST_SHLIBS = libtskm

RPC_API := TSKM

libtskm_SRCS = tskm_api_stub.c
libtskm_SRCS += tskm_api_lib.cpp

CPPFLAGS += -I. -I../../include-share -I./ -I../../server/include -I./../libtskmcfg/include

######## add compile option ########
CPPFLAGS += -fno-exceptions
LDFLAGS += -Wl,--no-as-needed
######### linked library (dynamic) #############
LDLIBS += -lrpc
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

include ../../../system_service.mk
