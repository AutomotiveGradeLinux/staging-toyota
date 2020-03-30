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
#######################################
INST_SHLIBS := libss_data_init

MKDATAINIT=mkdatainit.sh

INST_HEADERS = tskm_data_init.h task_manager_libss_data_init.h

VPATH = ./include/$(COMPONENT_NAME)
CPPFLAGS += -I./include/ -I./../../server/include -I./../libtskmcfg/include

libss_data_init_SRCS := tskm_data_init.cpp
libss_data_init_PCOS := $(shell ls ./obj/*.os)

$(libss_data_init_SRCS): $(libss_data_init_PCOS)
	set -e ; \
	sh $(MKDATAINIT)

######## add compile option ########
LDFLAGS += -Wl,--no-as-needed
CPPFLAGS += -fno-exceptions

CLEAN_FILES=$(libss_data_init_SRCS)

LDLIBS = -Wl,-Bdynamic -lNS_FrameworkUnified

include ../../../system_service.mk
