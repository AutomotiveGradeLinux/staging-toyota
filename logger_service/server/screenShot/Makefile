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
INST_PROGS = SS_ScrShot

######### installed library(*.a) #############


######### installed shared library(*.so) #############


######### install headers(*.h)  #############


######### compiled sources  #############
SS_ScrShot_SRCS = ss_logger_scrshot.cpp

######### add source path #############


######### add compile option #############
CPPFLAGS += -DLINUX
CPPFLAGS += -fno-exceptions

######### linked library #############


######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -ldu
LDLIBS += -Wl,-Bdynamic -ljpeg
#LDLIBS += -Wl,-Bdynamic -lextension
LDLIBS += -Wl,-Bdynamic -ldrm

######### add library path #############
LDFLAGS += -Wl,--gc-sections

LINK_CXX=Y
include ../../../system_service.mk
