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

INST_SHLIBS = libvp

VPATH = ./src ./include/$(COMPONENT_NAME)

INST_HEADERS = VP_GetEnv.h env_vehicle.h \
               env_vehicleparameterlibrary.h env_vehicleparameterlibrary_serial.h env_ext.h \
               VP_FuncCheck_CanRcv.h vehicle_parameter_library.h

libvp_SRCS = VP_FuncCheck_CanRcv.c VP_GetEnv.c

LDLIBS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lns_backup

CPPFLAGS += -I./include
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security
CPPFLAGS += -Wconversion
CPPFLAGS += -Wpointer-to-int-cast
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat
CPPFLAGS += -DVP_FUNCCHECK_CANRCV_CANRCV_ACTIVATE

include ../../other_service.mk
