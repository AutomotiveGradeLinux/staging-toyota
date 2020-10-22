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

#HOST TOOLS
INST_SHLIBS = libsssmcfg

VPATH = ./ ../include/$(COMPONENT_NAME)

INST_HEADERS =  ss_system_manager_conf.h
INST_HEADERS += ss_package.h
INST_HEADERS += ss_access_if_private.h
INST_HEADERS += system_manager_config.h

libsssmcfg_SRCS = ss_system_manager_conf.cpp



CPPFLAGS += -I../include

LDFLAGS += -Wl,--no-as-needed

LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lSS_RomAccessIf
LDLIBS += -Wl,-Bdynamic -lssver

LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lvp

DUMMY_SHLIB += libSS_RomAccessIf.so
DUMMY_SHLIB += libSS_SystemIfUnified.so

include ../../../../system_service.mk
