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

######### add source path #############
VPATH += ./
VPATH += ./src
VPATH += ./include
VPATH += ../server/include
VPATH += ../server/include/system_service

######### install headers(*.h)  #############
INST_HEADERS += resm.h
INST_HEADERS += resm_type.h
INST_HEADERS += resource_manager.h

######### installed program #############


######### installed library(*.a) #############
INST_LIBS = 

######### installed shared library(*.so) #############
INST_SHLIBS = libresm
RPC_API := RESMGR

######### compiled sources  #############
libresm_SRCS += resmgr_api_stub.c
libresm_SRCS += resmgr_api_lib.c

######### add include path #############
CPPFLAGS += -I./
CPPFLAGS += -I./include
CPPFLAGS += -I../server/include

######## add compile option ########

######### add library path #############
LDFLAGS += -Wl,--no-as-needed

######### linked library (dynamic) #############
LDLIBS += -lrpc
LDLIBS += -Wl,-Bdynamic -lev
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

include ../../system_service.mk
