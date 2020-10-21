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

######### installed shared library(*.so) #############
INST_SHLIBS = libVehicle_API


######### installed static library(*.a) #############
INST_LIBS = libVehicle_API


######### install headers(*.h)  #############
VPATH += ./include/$(COMPONENT_NAME)
INST_HEADERS = Vehicle_API.h vehicle_notifications.h Vehicle_Sensor_Common_API.h

######### compiled sources  #############
libVehicle_API_SRCS = Vehicle_API.cpp

######### add source path #############
VPATH += ./src

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./include/stub

######### add compile option #############
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

#### FOR DEBUG OPTIONS

######### add library path #############
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

include ../../stub.mk
