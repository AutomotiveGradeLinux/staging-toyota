#
# @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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
TARGET := libboot_hal.so

HEADER := hal_api/boot_hal.h

######### include paths/files ###################
CXXFLAGS := -I./inc/
CXXFLAGS += -I./hal_api/
CXXFLAGS += -I$(SDKTARGETSYSROOT)/usr/agl/include
CXXFLAGS += -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h

######### compile options #######################
CXXFLAGS += -Wall -fPIC

######### link options ##########################
LDFLAGS += -L$(SDKTARGETSYSROOT)/usr/agl/lib
LDFLAGS += -shared
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

RPATH :=

######### linked library ########################
LIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

######### source files ##########################
VPATH := src
OBJS := boot_hal.o

######### make targets ##########################
all: $(TARGET)
	@echo $@ done.

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -Wl,-rpath=$(RPATH) $(OBJS) $(LIBS) -o $(TARGET)
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
