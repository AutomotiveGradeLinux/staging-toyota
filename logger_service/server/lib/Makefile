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

######### installed program #############


######### installed library(*.a) #############
INST_LIBS = libSS_LoggerStoreLogs

######### installed shared library(*.so) #############


######### install headers(*.h)  #############


######### compiled sources  #############
libSS_LoggerStoreLogs_SRCS += ss_logger_store_logs.cpp

######### add source path #############


######### add include path #############
CPPFLAGS += -I./
CPPFLAGS += -I../include

CPPFLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
 
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### add compile option #############
CPPFLAGS += -DLINUX
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30
CPPFLAGS += -fno-exceptions

######### linked library #############


######### linked library (dynamic) #############


######### add library path #############
LDFLAGS += -g -shared
LDFLAGS += -Wl,--no-as-needed 
include ../../../system_service.mk
