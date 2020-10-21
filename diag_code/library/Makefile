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

######### installed program #############

######### installed library(*.a) #############

######### installed shared library(*.so) #############
INST_SHLIBS = libDiagCodeAPI

######### install headers(*.h)  #############
VPATH += ./include/$(COMPONENT_NAME)
INST_HEADERS += DiagCodeAPI.h DIAGCODE.h ss_diag.h

######### compiled sources  #############
libDiagCodeAPI_SRCS += DiagCodeAPI.cpp

######### add source path #############
VPATH += ./src

######### add include path #############
CPPFLAGS += -I./include/stub
CPPFLAGS += -I./../library_dummy_dccc/include/

######### add compile option #############
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat
######### dynamic linked library path #############

######### add library path #############


include ../../stub.mk
