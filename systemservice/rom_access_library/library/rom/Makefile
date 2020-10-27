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

######### search paths #############
VPATH += src
VPATH += ../include/$(COMPONENT_NAME)


######### installed shared library(*.so) #############
INST_SHLIBS = libSS_RomAccessIf

######### headers to be installed(*.h) #############
INST_HEADERS = rom_access_library.h ss_sm_boot_access.h ss_sm_ram_access.h ss_sm_rom_access.h

######### compiled sources #############
libSS_RomAccessIf_SRCS += ss_sm_rom_access.cpp ss_sm_boot_access.cpp ss_sm_ram_access.cpp ss_sm_checksum.cpp

######### add include path #############
CPPFLAGS += -I./../include

######### frameworkunifiedlog options #############
CPPFLAGS += -DLINUX
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3
LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

######### add compile option #############
LDFLAGS += -L./../nor/
LINK_SHLIB_CXX := y
LDFLAGS += -g -Wl,--gc-sections -shared
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### linked library (dynamic) #############
LDLIBS +=  -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS +=  -Wl,-Bdynamic -lcommon
LDLIBS +=  -Wl,-Bdynamic -lssaccess
LDLIBS +=  -Wl,-Bdynamic -lextension

CPPFLAGS_ss_sm_checksum.o += -fno-exceptions
CPPFLAGS_ss_sm_checksum.os += -fno-exceptions
CPPFLAGS_ss_sm_ram_access.o += -fno-exceptions
CPPFLAGS_ss_sm_ram_access.os += -fno-exceptions

include ../../../system_service.mk
