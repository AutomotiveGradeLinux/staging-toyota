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

# QCC arm COMPILER SETTINGS
#  Standard form of tool.mk file configured for PosixBasedOS001 arm v7
#  Note: Make sure PosixBasedOS001_HOST is set to install version desired

# Tools and Options
LIB_EXT = a
SO_EXT = so
LIB_PREFIX = lib
OBJ_EXT = o
BIN_EXT =
CC = ${PosixBasedOS001_HOST}/usr/bin/qcc -c
CPP = ${PosixBasedOS001_HOST}/usr/bin/qcc -c
AS = ${PosixBasedOS001_HOST}/usr/bin/ntoarmv7-as
AR = ${PosixBasedOS001_HOST}/usr/bin/ntoarmv7-ar
OBJCPY := ${PosixBasedOS001_HOST}/usr/bin/ntoarmv7-objcopy
LD = qcc
# Add addtion Linker Flags....
# If your application contains c++ code you will need
# the -lang-c++
LD_CPP = -lang-c++ #-lm

# Compile options and commands
TARGET_FLG = -Vgcc_ntoarmv7le

# Define Map file Option
MAP_FILE_FLG=-M

# Set Debug Flag
ifdef DEBUG
CC_DEBUG_FLG = -g
DEBUG_EXT=_g
endif

# Set Profiling Flag
ifdef PROFILING
CC_PROFILING_FLG = -p
endif

# Set Optimization Flag (precident to force none, force full, force basic, or default remain as defined)
ifeq ($(OPTIMIZATION),OPTIMIZE)
CC_OPT_FLG = -O2
else
ifeq ($(OPTIMIZATION),FULL_OPTIMIZE)
CC_OPT_FLG = -O3
else
ifeq ($(OPTIMIZATION),NONE)
CC_OPT_FLG =
endif
endif
endif


# Compiler Options
CC_OFLAG = -o
CC_IFLAG = -I
CC_FLAGS = $(TARGET_FLG) -w9 -Wc,-Wall -shared $(CC_OPT_FLG) $(CC_PROFILING_FLG) $(CC_DEBUG_FLG)
CC_TARGET = $(@)
CC_CMD = $(CC) $(CC_FLAGS) $(DEFS) $(INCLUDES) $(@F:.$(OBJ_EXT)=.c) -o $(@)
CPP_CMD = $(CPP) $(CC_FLAGS) $(DEFS) $(INCLUDES) $(@F:.$(OBJ_EXT)=.cpp) -o $(@)
CPP_D_CMD = arm-unknown-nto-PosixBasedOS001.5.0eabi-gcc.exe -c $(DEFS) $(INCLUDES) -MM -MP $< > $(@:.o=.d)

# Archive Options  (Static Library)
AR_OFLAG =
AR_FLAGS = -r
AR_CMD = $(AR) $(AR_FLAGS) $(AR_OFLAG)$(@) $^

# Linker Options
# Executable
# Used in the form of $(LD_CMD) -L[Library Paths] -l[libraries] [objects] $(LD_OFLAG) executable_name.$(BIN_EXT)
LD_FLAGS = $(TARGET_FLG) $(CC_PROFILING_FLG) $(CC_DEBUG_FLG) $(LD_CPP)
LD_OFLAG = -o
LD_CMD = $(LD) -L$(LIB_PATH) $(LD_FLAGS) $(MAP_FILE_FLG)
EXEC_CMD = $(LD_CMD) $(LD_OFLAG) $(@) $(OBJECTS) $(addprefix -L , $(LIB_PATHS)) \
		-Bstatic $(addprefix -l , $(STATIC_LIBS)) -Bdynamic $(addprefix -l , $(DYNAMIC_LIBS))

# To create shared objects use the linker command but add -shared
SO_CMD = $(LD) $(LD_FLAGS) -shared $(MAP_FILE_FLG)
SLIB_CMD = $(SO_CMD) $^ $(LD_OFLAG) $(@) $(addprefix -L, $(LIB_PATHS)) \
		-Bstatic $(addprefix -l,$(STATIC_LIBS)) -Bdynamic $(addprefix -l,$(DYNAMIC_LIBS))

