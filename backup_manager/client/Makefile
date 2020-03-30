#
# @copyright Copyright (c) 2017-2019 TOYOTA MOTOR CORPORATION.
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

######### libraries to be installed(*.so)#############
INST_SHLIBS := libns_backup

######### headers to be installed(*.h)   #############
INST_HEADERS := backup_manager.h ns_backup.h

######### sources to be compiled         #############
libns_backup_SRCS := bkup_api.cpp

######### search paths                   #############
VPATH += ./src ./include/$(COMPONENT_NAME) ../server/include

######### include paths                  #############
CPPFLAGS += -I./include
CPPFLAGS += -I../server/include

######### frameworkunifiedlog options                 #############
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3

LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

######### libraries to be linked(dynamic)#############
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

######### other unit specific options    #############
LINK_CXX=Y

CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

include ../../native_service.mk
