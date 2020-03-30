#
# @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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
INST_SHLIBS = libPOS_base_API

######### install headers(*.h)  #############
VPATH += ./include/$(COMPONENT_NAME)
INST_HEADERS  += std_types.h
INST_HEADERS  += sys_timerapi.h
INST_HEADERS  += WPF_STD.h
INST_HEADERS  += pos_message_header.h
INST_HEADERS  += positioning_base_library.h
INST_HEADERS  += positioning_positioningbaselibrarylog.h

######### compiled sources  #############
VPATH += src
libPOS_base_API_SRCS += _pbTimer.cpp
libPOS_base_API_SRCS += _pbCSection.cpp
libPOS_base_API_SRCS += _pbMutex.cpp
libPOS_base_API_SRCS += _pbSem.cpp
libPOS_base_API_SRCS += _pbDram.cpp
libPOS_base_API_SRCS += _pbFsys.cpp
libPOS_base_API_SRCS += _pbMem.cpp
libPOS_base_API_SRCS += _pbSram.cpp
libPOS_base_API_SRCS += _pbOSCtrl.cpp
libPOS_base_API_SRCS += _pbSerial.cpp
libPOS_base_API_SRCS += _pbWaitforsingleobject.cpp
libPOS_base_API_SRCS += _pbOther.cpp
libPOS_base_API_SRCS += _pbProcMng.cpp
libPOS_base_API_SRCS += _pbCommon.cpp
libPOS_base_API_SRCS += _CWORD64_api.cpp
libPOS_base_API_SRCS += _pbMisc.cpp
libPOS_base_API_SRCS += _pbEvent.cpp
libPOS_base_API_SRCS += _pbMsg.cpp
libPOS_base_API_SRCS += _pbSum.cpp
libPOS_base_API_SRCS += MsgBapi.cpp
libPOS_base_API_SRCS += memcpy_64p_sync.cpp
libPOS_base_API_SRCS += memset_64p_sync.cpp

######### add include path #############
CPPFLAGS += -I./include
CPPFLAGS += -I./

######### add compile option #############
CPPFLAGS += -DLINUX -fPIC
CPPFLAGS += -D_CWORD64_API_DOES_NOT_USE_UNICODE

LDFLAGS += -Wl,--no-undefined
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -lrt
LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lev

######### add library path #############
LDFLAGS += -shared

include  ../../vehicle_service.mk
