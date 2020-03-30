#
# @copyright Copyright (c) 2017-2019 TOYOTA MOTOR CORPORATION.
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
INST_PROGS = Positioning

######### compiled sources  #############
Positioning_SRCS += ps_main.cpp
Positioning_SRCS += positioning_application.cpp

#ifeq ($(ARCH),arm64)
#LDLIBS += -Wl,-Bdynamic -lpositioning_hal
#else
#LDLIBS += -Wl,-Bdynamic -lpositioning_hal
#endif  #($(ARCH),arm64)

######### add include path #############
CPPFLAGS += -I../../../../
CPPFLAGS += -I../../../client/include
CPPFLAGS += -I../../include/common/
CPPFLAGS += -I../../include/Sensor/
CPPFLAGS += -I../../include/ServiceInterface/
CPPFLAGS += -I../../include/nsfw/
#CPPFLAGS += -I../../../../diag_code/library/include

######### add compile option #############
CPPFLAGS += -DLINUX
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### linked library(static) #############
LDLIBS += -Wl,-Bstatic -lPOS_Sensor
LDLIBS += -Wl,-Bstatic -lPOS_ServiceInterface

# LDLIBS += -Wl,-Bstatic -lVehicle_API

######### linked library (dynamic) #############
#ifeq (arm64, $(ARCH))
#LDLIBS += -Wl,-Bdynamic -lpositioning_hal
#LDLIBS += -Wl,-Bdynamic -lVehicle_API
#endif
#LDLIBS += -Wl,-Bdynamic -lClock_API
#LDLIBS += -Wl,-Bdynamic -lMM_DREC_API
#LDLIBS += -Wl,-Bdynamic -lextension
LDLIBS += -Wl,-Bdynamic -lPOS_base_API
LDLIBS += -Wl,-Bdynamic -lPOS_common_API
LDLIBS += -Wl,-Bdynamic -lPOS_gps_API
LDLIBS += -Wl,-Bdynamic -lPOS_sensor_API
LDLIBS += -Wl,-Bdynamic -lz
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lns_backup
LDLIBS += -Wl,-Bdynamic -lssver
LDLIBS += -Wl,-Bdynamic -lstdc++
#LDLIBS += -Wl,-Bdynamic -lDiagCodeAPI
#LDLIBS += -Wl,-Bdynamic -lDTime_Api
#LDLIBS += -Wl,-Bdynamic -lVehicle_API
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -lev
#LDLIBS += -Wl,-Bdynamic -lCommUSB

######### add library path #############
#LDFLAGS += -L../../positioning_hal
LDFLAGS += -L../Sensor
LDFLAGS += -L../ServiceInterface
LDFLAGS += -L../../../client/src/POS_common_API
LDFLAGS += -L../../../client/src/POS_gps_API
LDFLAGS += -L../../../client/src/POS_sensor_API

include ../../../../vehicle_service.mk
