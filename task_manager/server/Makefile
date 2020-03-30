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


#######################################
INST_PROGS = tskmgr

######### add source path #############
######### add local value #############
VPATH = ./src ./../client/libtskm ./../client/libtskmcfg/include/$(COMPONENT_NAME)

tskmgr_SRCS += tskm_main.cpp

######### compiled sources  #############
tskmgr_SRCS +=   tskm_api.cpp \
                 tskm_state.cpp \
                 tskm_wakeup.cpp \
                 tskm_shutdown.cpp \
                 tskm_svc.cpp \
                 tskm_watch.cpp \
                 tskm_debug.cpp \
                 tskm_comm.cpp \
                 tskm_port_pf.cpp \
                 tskm_port_subsys.cpp \
                 tskm_srvr_stub.cpp



######### add include path #############
CPPFLAGS = -I./include/ -I./../include-share/ -I./../client/ss_data_init/include/ -I./../client/libtskmcfg/include/ -I./../client/libtskm/
######### add compile option ###########
CPPFLAGS += -fno-exceptions
CPPFLAGS += -DIMPL_AGL_APPLICATION_CALLBACKS_PRE_BACKGROUND
######### add library path #############
LDFLAGS  += -L./../client/ss_data_init -L./../client/libtskmcfg

######### linked library (dynamic) #############
LDLIBS += -Wl,-Bdynamic -lstdc++
LDLIBS += -Wl,-Bdynamic -lrt # for mq_xxx
LDLIBS += -Wl,-Bdynamic -lss_data_init
LDLIBS += -Wl,-Bdynamic -lSS_SystemIfUnified
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified
LDLIBS += -Wl,-Bdynamic -lresm
LDLIBS += -Wl,-Bdynamic -lcommon
LDLIBS += -Wl,-Bdynamic -lvp
LDLIBS += -Wl,-Bdynamic -lrpc -lev

#LDLIBS += -Wl,-Bdynamic -lDiagCodeAPI
LDLIBS += -Wl,-Bdynamic -ltskmcfg


LINK_CXX=Y

include ../../system_service.mk
