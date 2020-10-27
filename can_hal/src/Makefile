#
# @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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

######### installed shared library(*.so) #############
INST_SHLIBS = libcan_hal

######### installed static library(*.a) #############

######### install headers(*.h)  #############
INST_HEADERS = ../hal_api/can_hal.h

######### compiled sources  #############
libcan_hal_SRCS = can_hal_core.cpp can_hal_stm.cpp can_hal_api.cpp
libcan_hal_SRCS += driver_can__CWORD31_.c

######### include paths/files ###################
CPPFLAGS += -I./inc/
CPPFLAGS += -I../hal_api/

######### compile options #######################
CPPFLAGS += -Wall -fPIC

######### link options ##########################
LDFLAGS += -shared
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -L$(SDKTARGETSYSROOT)/usr/agl/lib

RPATH := /usr/lib:/usr/agl/lib

######### linked library ########################
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lpthread
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

COMPONENT_NAME =
AGL_TOPDIR ?= $(DESTDIR)/usr
include ../can_hal.mk
