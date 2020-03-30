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

######### installed library(*.a) #############
INST_LIBS = libPOS_ServiceInterface

######### compiled sources  #############
libPOS_ServiceInterface_SRCS += BackupMgrIf.cpp
libPOS_ServiceInterface_SRCS += ClockIf.cpp
libPOS_ServiceInterface_SRCS += CommUsbIf.cpp
libPOS_ServiceInterface_SRCS += DevDetectSrvIf.cpp
libPOS_ServiceInterface_SRCS += DiagSrvIf.cpp
libPOS_ServiceInterface_SRCS += PSMShadowIf.cpp
libPOS_ServiceInterface_SRCS += VehicleIf.cpp

libPOS_ServiceInterface_SRCS += VehicleIf.cpp

######### add include path #############
CPPFLAGS += -I../../include/common/
CPPFLAGS += -I../../include/nsfw/
CPPFLAGS += -I../../include/ServiceInterface

CPPFLAGS += -I../../../client/include
CPPFLAGS += -I../../../client/src/Vehicle_API/common
CPPFLAGS += -I../../../client/src/POS_sensor_API/common
CPPFLAGS += -I../../../client/src/POS_gps_API
CPPFLAGS += -I../../../client/src/POS_naviinfo_API/common
CPPFLAGS += -I../../../client/src/CanInput_API/common
CPPFLAGS += -I../../../client/src/SensorLocation_API/common

#CPPFLAGS += -I../../../../diag_code/library/include

######### add compile option #############
CPPFLAGS += -DLINUX

LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### add library path #############
LDFLAGS +=

include ../../../../vehicle_service.mk

