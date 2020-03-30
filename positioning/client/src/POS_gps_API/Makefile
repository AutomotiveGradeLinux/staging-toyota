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
######### installed shared library(*.so) #############
INST_SHLIBS = libPOS_gps_API

######### compiled sources  #############
VPATH += ../POS_naviinfo_API/variant/awnavi/src

libPOS_gps_API_SRCS += Gps_API.cpp
libPOS_gps_API_SRCS += Naviinfo_API.cpp

######### add include path #############
CPPFLAGS += -I./
CPPFLAGS += -I../../../server/include/common
CPPFLAGS += -I../../include
CPPFLAGS += -I../../../server/include/nsfw

CPPFLAGS += -I../Vehicle_API
CPPFLAGS += -I../Vehicle_API/common

######### add compile option #############
CPPFLAGS += -DLINUX -fPIC

LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lPOS_base_API
LDLIBS += -Wl,-Bdynamic -lvp

######### add library path #############
LDFLAGS += -shared

include ../../../../vehicle_service.mk
