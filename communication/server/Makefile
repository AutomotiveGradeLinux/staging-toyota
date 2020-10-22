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

######### add source path #############

######### installed program #############
INST_PROGS = communication

######### installed library(*.a) #############

######### installed shared library(*.so) #############

######### compiled sources  #############

## from ./src/main
communication_SRCS += ./src/main/communication_main.cpp
communication_SRCS += ./src/main/communication_application.cpp

## from ./src/threads
communication_SRCS += ./src/threads/CAN_Thread.cpp
communication_SRCS += ./src/threads/Thread_Common.cpp

## from	./src/CAN/CommWatch/
communication_SRCS += ./src/CAN/CommWatch/CAN_CommWatch.cpp

## from ./src/CAN/Delivery/
communication_SRCS += ./src/CAN/Delivery/CAN_Delivery.cpp

## from ./src/CAN/TxMsg/
communication_SRCS += ./src/CAN/TxMsg/CAN_TxMsg.cpp

######### add source path #############
VPATH += \
	./src/CAN/Command \
	./src/CAN/CommWatch  \
	./src/CAN/Delivery  \
	./src/CAN/main  \
	./src/CAN/TxMsg  \
    ./src/main  \
	

######### add include path #############
CPPFLAGS += \
	-I./../client_can/include \
	-I./include \
	-I./include/CAN/Command  \
	-I./include/CAN/CommWatch  \
	-I./include/CAN/Delivery  \
	-I./include/CAN/main  \
	-I./include/CAN/TimerCtrl  \
	-I./include/CAN/TxMsg  \
	-I./include/main  \
	-I./include/threads  \
	-I./include/private

######### add compile option #############
CPPFLAGS += -DLINUX -fdata-sections -ffunction-sections -DFLG_CORE

CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security 

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

######### add library path #############
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--gc-sections

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lPosixBasedOS001legacy
LDLIBS += -Wl,-Bdynamic -lrt
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lcan_hal

include ../../peripheral_service.mk
