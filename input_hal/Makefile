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

COMPONENT_NAME := peripheral_service

######### installed program/file ################
TARGET := libinput_hal.so

HEADER := ./hal_api/input_hal.h

######### include paths/files ###################
CXXFLAGS := -I./inc/
CXXFLAGS += -I./hal_api/
CXXFLAGS += -I$(SDKTARGETSYSROOT)/usr/agl/include
CXXFLAGS += -I$(SDKTARGETSYSROOT)/usr/include/libdrm
CXXFLAGS += -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h

######### compile options #######################
CXXFLAGS += -Wall -fPIC
CXXFLAGS += -D_USE_DRM

######### link options ##########################
LDFLAGS := -shared
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -L$(SDKTARGETSYSROOT)/usr/agl/lib

######### linked library ########################
LIBS := -Wl,-Bdynamic -lstdc++
LIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LIBS += -Wl,-Bdynamic -ludev
LIBS += -Wl,-Bdynamic -lpthread
LIBS += -Wl,-Bdynamic -ldrm

######### source files ##########################
VPATH := src
OBJS := input_hal.o
OBJS += input_touch_ilitek.o
OBJS += input_util.o
OBJS += input_udev_monitor.o
OBJS += input_drm.o

######### make targets ##########################
all: $(TARGET)
	@echo $@ done.

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)
	@echo $@ done.

######### make cleans ###########################
GCNO_FILES := $(filter %.gcno,$(OBJS:%.o=%.gcno))

clean:
	rm -f $(OBJS) $(TARGET) $(GCNO_FILES)
	@echo $@ done.

######### make installs #########################
install: install-input
install:
	install -d -m 0755 $(DESTDIR)/usr/lib
	install -m 0755 $(TARGET) $(DESTDIR)/usr/lib
	@echo $@ done.

install-input:
	install -m 0755 -d $(DESTDIR)/usr/agl/include/$(COMPONENT_NAME)/
	install -m 0644 ./hal_api/$(COMPONENT_NAME)/aglinput.h $(DESTDIR)/usr/agl/include/$(COMPONENT_NAME)/
	install -m 0644 ./hal_api/$(COMPONENT_NAME)/extinput.h $(DESTDIR)/usr/agl/include/$(COMPONENT_NAME)/
