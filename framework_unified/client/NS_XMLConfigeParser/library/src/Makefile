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


######### installed shared library(*.so) #############
INST_SHLIBS = libNS_XMLConfigParser

######### install headers(*.h)  #############


######### compiled sources  #############
libNS_XMLConfigParser_SRCS = ns_xml_reader.cpp ns_xml_writer.cpp ns_xmlparser.cpp ns_xmlparser_attributes.cpp ns_xmlparser_node.cpp

######### add source path #############

######### add include path #############

CPPFLAGS += -I$(SDKTARGETSYSROOT)/usr/include/libxml2
CPPFLAGS += -I../../../include
CPPFLAGS += -I../include
CPPFLAGS += -I../../../include
#inc_dummy
CPPFLAGS += -I../../../NS_ConfigParser/include

######### add compile option #############
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3

######### add library path #############
LDFLAGS += -L../../../
LDFLAGS += -Wl,--no-as-needed
######### linked library (dynamic) #############
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lxml2
LDLIBS += -Wl,-Bdynamic -lm

LINK_SHLIB_CXX = Y

include ../../../../../native_service.mk
