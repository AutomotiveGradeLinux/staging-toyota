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

SUBDIRS = libtskmcfg ss_data_init libtskm libprimary

VPATH = ../server/include/$(COMPONENT_NAME)

######### install headers(*.h)  #############
INST_HEADERS =  INI_API.h INI_API.hpp  Primary_common.h  tskm.h  tskm_type.h tskm_local_type.h
INST_HEADERS += sysup_from.h sysup.h tskm_svc.h
INST_HEADERS += task_manager.h

include ../../system_service.mk
