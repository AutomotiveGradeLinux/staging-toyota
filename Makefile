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
TARGET   = libvideo_in_hal.so

HEADER   = hal_api/video_in_hal.h

######### include paths/files ###################
CFLAGS += -I./hal_api/
CFLAGS += -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h

######### compile options #######################
CFLAGS += -Wall -fPIC

######### link options ##########################
LDFLAGS += -shared
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--warn-unresolved-symbols

######### linked library ########################

######### source files ##########################
VPATH := ./src/

OBJS := video_in_hal.o

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
