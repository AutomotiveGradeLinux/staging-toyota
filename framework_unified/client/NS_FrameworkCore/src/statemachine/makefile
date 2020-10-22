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

#
# Standard Module Makefile version 2.0
#

COMPONENT_NAME = NS_FrameworkCore

include $(PRJ_ROOT)cfg/depends.mk

# Additive Compile Flags (Flags from initiating make process will still apply)
DEFS +=


INCLUDES = \
	$(CFG_INCS) \
	$(CC_IFLAG). \
	$(CC_IFLAG)../../inc \
	$(CC_IFLAG)../../inc/statemachine \
	$(DEPENDS_INCLUDES) \


## Sources Section

SOURCES = \
	$(wildcard *.c*)

#
# Convert the source files to object files with correct folder location.
#
#
C_LANG_OBJECTS = $(addprefix $(BLD_PATH),$(addsuffix .$(OBJ_EXT),$(basename $(filter %.c ,$(SOURCES) ) ) ) )
CPP_LANG_OBJECTS = $(addprefix $(BLD_PATH),$(addsuffix .$(OBJ_EXT),$(basename $(filter %.cpp %.cc %.cxx,$(SOURCES) ) ) ) )


# List of all sources to be generated.  Can be assembled from the other defintitions.
OBJECTS = \
	$(C_LANG_OBJECTS) \
	$(CPP_LANG_OBJECTS)


# All headers that are dependencies.  Wildcard is easy to pickup local headers.
#  This is only to automate the rebuilding, all builds on the servers are cleans
#  So this is not a huge deal when building on a component level.
HEADERS = \
	$(wildcard *.h) \
	$(wildcard ../../inc/*.h) \
	$(wildcard ../../inc/NS_Statemachine/*.h) \
	$(wildcard $(REPOSITORY_ROOT)NativeServices/inc/*.h) \

# Make targets
#  Standard
all: banner module_dirs local

base: banner module_dirs subdirs local

binary: base

local: $(OBJECTS)

# Standard set of derived targets
library: base \
	$(LIBRARIES)
	@echo "***** `date` Done building library: $(COMPONENT_NAME) ******"

# Defines specific for each deliverable


# Default source file build rules
$(OBJECTS): $(HEADERS)

# Standard Building of Source Files (Default builds for all objects defined above)
$(C_LANG_OBJECTS): $(SOURCES) $(HEADERS)
	$(CC_CMD)

$(CPP_LANG_OBJECTS): $(SOURCES) $(HEADERS)
	$(CPP_CMD)


clean:
	-rm -f $(BINARIES)
	-rm -f $(LIBRARIES)
	-rm -f $(OBJECTS)

module_dirs: build_dirs
