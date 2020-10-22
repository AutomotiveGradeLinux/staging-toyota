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

######### programs to be installed       #############
INST_PROGS := NS_BackupMgr

######### sources to be compiled         #############
NS_BackupMgr_SRCS := backupmanager_main.cpp \
		     bkup_process.cpp \
		     bkup_param.cpp \
		     bkup_crc32.cpp \
		     backupmanager_application.cpp

######### search paths                   #############
VPATH += ./src ./include ../client/include/$(COMPONENT_NAME)

######### include paths                  #############
CPPFLAGS += -I./include
CPPFLAGS += -I../client/include
CPPFLAGS += -I$(SDKTARGETSYSROOT)/usr/include/libxml2

######### frameworkunifiedlog options                 #############
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 -DFRAMEWORKUNIFIEDLOGAPPZONES=31,30,29,28,27,26,9,8,3

LDFLAGS += -Wl,--no-as-needed
LDFLAGS += -Wl,--no-undefined

######### libraries to be linked(dynamic)#############
LDLIBS += -Wl,-Bdynamic \
          -lSS_SystemIfUnified \
          -lNS_FrameworkUnified \
          -lxml2 \
          -lrt \
          -lnv_hal \
          -lvp

######### other unit specific options    #############
LINK_CXX=Y

CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND

install-data: install_data

install_data:
	install -d -m 755 $(DESTDIR)/nv/BS/ns/backup_manager/rwdata

include ../../native_service.mk
