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

######### installed shared library(*.so) #############
INST_SHLIBS += libextension

######### install headers(*.h)  #############
INST_HEADERS += el_gpio.h el_mem.h

######### compiled sources  #############
libextension_SRCS += el_gpio.c el_mem.c 

######### add source path #############
VPATH += src
VPATH += include/stub

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./include/$(COMPONENT_NAME)

######### add compile option #############
CPPFLAGS += -std=c++11

######### linked library #############

######### linked library (dynamic) #############

######### add library path #############


include ../../stub.mk
