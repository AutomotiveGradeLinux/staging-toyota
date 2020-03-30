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

######### installed program #############
INST_PROGS = drop_caches

######### install headers(*.h)  #############
INST_HEADERS = 

######### compiled sources  #############
drop_caches_SRCS = ./src/drop_caches.cpp

######### add include path #############
CPPFLAGS = -I../server/include

######## define options ########
CPPFLAGS += -DFRAMEWORKUNIFIEDLOGOPTIONS=0x08 \
            -DFRAMEWORKUNIFIEDLOGAPPZONES=31
CPPFLAGS += -fno-exceptions
LDFLAGS += -Wl,--no-as-needed
######### linked library (dynamic) #############
LDLIBS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified

include ../../system_service.mk
