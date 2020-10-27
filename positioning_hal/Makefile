#
# @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

######### installed program/file ################
TARGET := libpositioning_hal.so

HEADER := hal_api/gps_hal.h hal_api/positioning_hal.h

######### include paths/files ###################
CXXFLAGS += -I.
CXXFLAGS += -I./inc/Common
CXXFLAGS += -I./inc/GpsCommon
CXXFLAGS += -I./inc/LineSensDrv
CXXFLAGS += -I./hal_api/
CXXFLAGS += -I$(SDKTARGETSYSROOT)/usr/agl/include/
CXXFLAGS += -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h


######### compile options #######################
CXXFLAGS += -Wall -fPIC
CXXFLAGS += -DLINUX -D__CWORD71__VEHICLE_ -D__CWORD71_

######### link options ##########################
LDFLAGS += -shared
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -L$(SDKTARGETSYSROOT)/usr/agl/lib

RPATH := /usr/lib

######### linked library ########################
LIBS += -Wl,-Bdynamic -lstdc++
LIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
#LIBS += -Wl,-Bdynamic -lgps_mng
LIBS += -Wl,-Bdynamic -lPOS_base_API
LIBS += -Wl,-Bdynamic -lns_backup

######### source files ##########################
VPATH := src/Common
VPATH += src/GpsCommon
VPATH += src/LineSensDrv

OBJS += positioning_common.o
OBJS += positioning_hal.o
OBJS += MDev_Gps_API.o
OBJS += MDev_Gps_Main.o
OBJS += MDev_GpsRecv.o
OBJS += MDev_Gps_Nmea.o
OBJS += MDev_Gps_Common.o
OBJS += MDev_Gps_Mtrx.o
OBJS += MDev_Gps_TimerCtrl.o
OBJS += LineSensDrv_Sensor.o
OBJS += LineSensDrv_Thread.o
OBJS += LineSensDrv_Api.o
OBJS += MDev_GpsRollOver.o

######### make targets ##########################
all: $(TARGET)
	@echo $@ done.

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -Wl,-rpath=:$(RPATH) $(OBJS) $(LIBS) -o $(TARGET)
	@echo $@ done.

######### make cleans ###########################
GCNO_FILES := $(filter %.gcno,$(OBJS:%.o=%.gcno))

clean:
	rm -f $(OBJS) $(TARGET) $(GCNO_FILES)
	@echo $@ done.

######### make installs #########################
install:
	install -d -m 0755 $(DESTDIR)/usr/lib
	install -m 0755 $(TARGET) $(DESTDIR)/usr/lib
	@echo $@ done.


