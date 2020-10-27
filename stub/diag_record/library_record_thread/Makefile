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

######### installed library(*.a) #############
INST_LIBS = libPFDRECThread

######### install headers(*.h)  #############
INST_HEADERS += pfdrec_thread_ifc.h

######### compiled sources  #############
libPFDRECThread_SRCS += pfdrec_thread.cpp

######### add source path #############
VPATH += ./src ./include/$(COMPONENT_NAME)

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./include/stub
CPPFLAGS += -I./../library_record_api/include

######### add compile option #############
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### linked library (dynamic) #############
LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lstdc++

include ../../stub.mk
