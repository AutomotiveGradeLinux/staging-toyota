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

# installed shared library(.so)
INST_SHLIBS := libssver

# compiled source
libssver_SRCS := ss_ver.cc

# installed header
INST_HEADERS := version_library.h ss_ver.h

# additional source path
VPATH += ./src \
         ./include/$(COMPONENT_NAME)

# additional include path
CPPFLAGS += -I./include

# linked library
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic,--no-as-needed -lNS_FrameworkUnified

include ../../system_service.mk
