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
INST_SHLIBS = libCAN_API

######### install unit representative headers(*.h)  #############
VPATH += ../server/include/$(COMPONENT_NAME)
INST_HEADERS += communication_notifications.h communication.h ps_services.h

######### install headers(*.h)  #############
VPATH += ./include/$(COMPONENT_NAME)
INST_HEADERS += Canif_API.h communication_can.h

######### compiled sources  #############
libCAN_API_SRCS += Canif_API.cpp

######### add source path #############
VPATH += ./src

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./../client_lan/include/
CPPFLAGS += -I./../server/include
CPPFLAGS += -I./../server/include/private
CPPFLAGS += -I./../server/include/CAN/TxMsg
CPPFLAGS += -I./../server/include/CAN/main
CPPFLAGS += -I./../server/include/main
CPPFLAGS += -I./../server/include/threads

######### add compile option #############
CPPFLAGS += -DLINUX -fPIC -fdata-sections -ffunction-sections -DFLG_CORE

CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security 

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### add library path #############
LDFLAGS += -Wl,-M -Wl,--gc-sections
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

######### linked library (static) #############

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lrt 
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy

include ../../peripheral_service.mk
