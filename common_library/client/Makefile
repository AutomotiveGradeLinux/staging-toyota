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

INST_SHLIBS := libcommon

libcommon_SRCS := cl_process.c cl_sem.c cl_monitor.c cl_region.c cl_lock.c cl_cgroup.c
INST_HEADERS := common_library.h cl_process.h cl_sem.h cl_monitor.h cl_lock.h cl_region.h cl_lockid.h

VPATH = ./src ./include/$(COMPONENT_NAME)
 
CPPFLAGS += -I./include/

LDFLAGS += -Wl,--no-as-needed
 
LDLIBS += -lrt

include ../../native_service.mk
