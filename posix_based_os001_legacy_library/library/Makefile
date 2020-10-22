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

INST_SHLIBS = libPosixBasedOS001legacy

VPATH = ./src ./include/$(COMPONENT_NAME)/
INST_HEADERS = strlcat.h strlcpy.h \
itoa.h \
PosixBasedOS001TimeApi.h \
PosixBasedOS001ClockCycleApi.h \
ultoa.h \
posix_based_os001_legacy_library.h

libPosixBasedOS001legacy_SRCS += strlcat.c strlcpy.c \
itoa.c \
PosixBasedOS001TimeApi.c \
PosixBasedOS001ClockCycleApi.c \
ultoa.c

CPPFLAGS = -I./include

LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wpointer-to-int-cast
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat



include ../../other_service.mk
