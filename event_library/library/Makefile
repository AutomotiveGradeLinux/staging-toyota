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

INST_SHLIBS = libev
libev_SRCS := src/ev_lib.c
VPATH = ./src ./include/$(COMPONENT_NAME)/
INST_HEADERS = ev_lib.h event_library.h

CPPFLAGS = -I./include

FDLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wpointer-to-int-cast
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

LDFLAGS += -Wl,--no-as-needed
LDLIBS := -lpthread

include ../../other_service.mk
