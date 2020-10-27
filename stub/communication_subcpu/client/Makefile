#
# @copyright Copyright (c) 2019-2020 TOYOTA MOTOR CORPORATION.
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

######### installed shared library(*.so) #############
INST_SHLIBS = libCommSubCpu

######### install headers(*.h)  #############
INST_HEADERS = CommSubCpu_API.h com_message_header.h

######### compiled sources  #############
libCommSubCpu_SRCS += ./src/CommSubCpu_API.cpp

######### add source path #############
VPATH += src
VPATH += include/stub

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./include/$(COMPONENT_NAME)

######### add compile option #############

######### linked library #############

######### linked library (dynamic) #############

######### add library path #############


include ../../stub.mk
