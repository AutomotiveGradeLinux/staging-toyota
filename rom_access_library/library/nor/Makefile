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

######### search paths #############
VPATH += src
VPATH += ../include/$(COMPONENT_NAME)

######### installed shared library(*.so) #############
INST_SHLIBS = libssaccess

######### headers to be installed(*.h) #############
INST_HEADERS = nor_backup.h

######### compiled sources #############
libssaccess_SRCS = nor_backup.c

######### add include path #############
CPPFLAGS += -I../include/

######### frameworkunifiedlog options #############
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed

######### add compile option #############
LINK_SHLIB_CXX := y
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

include ../../../system_service.mk
