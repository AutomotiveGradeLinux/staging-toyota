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


######### installed library(*.a) #############

######### installed shared library(*.so) #############
INST_SHLIBS = libNS_BuildVersionCheck

######### install headers(*.h)  #############

######### compiled sources  #############
libNS_BuildVersionCheck_SRCS = NS_BuildVersionCheck.cpp

######### add source path #############
VPATH += ./src

######### add include path #############
CPPFLAGS += -I./../../../include/

######### add compile option #############
CPPFLAGS += -DLINUX -fdata-sections -ffunction-sections -DFLG_LEGACY

CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3
CPPFLAGS += -DBUILD_VERSION=PastModel002_stable_c1_I20130701-0000

CPPFLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -g -Wl,--gc-sections -shared

LDLIBS += -Wl,-Bdynamic -lstdc++

######### linked library #############

######### linked library (dynamic) #############

######### add library path #############

include ../../../../../native_service.mk
