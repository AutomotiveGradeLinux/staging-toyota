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
#  Standard form of tool.mk file configured for PosixBasedOS001 vmware

#Add PosixBasedOS001 binary tools to path
PATH := $(PosixBasedOS001_HOME)/usr/bin/;$(PATH)

# include the compiler definitions in which we are linting against
include $(PRJ_ROOT)/cfg/PosixBasedOS001_armv7.mk

# Tools and Options
CODE_COUNT = surveyor
LINT = lint-nt
CC_IFLAG = -i

CC_DEFS = -D__LANGUAGE_C -D_LANGUAGE_C -D__PosixBasedOS001__ -D__PosixBasedOS001__ \
	-D__GNUC__=4 -D__GNUC_MINOR__=4 -D__GNUC_PATCHLEVEL__=2 \
	-D__unix__ -D__unix -D__ELF__ -D__ARM__ -D__arm__ \
	-D__ARM_ARCH_4__ -D__APCS_32__ -D__LITTLEENDIAN__ -D__ARMEL__ -D_MUDFLAPTH

CPP_DEFS = -D__cplusplus -D__PosixBasedOS001__ -D__PosixBasedOS001__ \
	-D__GNUC__=4 -D__GNUC_MINOR__=4 -D__GNUC_PATCHLEVEL__=2 \
	-D__NO_INLINE__ -D__DEPRECATED -D__EXCEPTIONS \
	-D__unix__ -D__unix -D__ELF__ -D__ARM__ -D__arm__ \
	-D__ARM_ARCH_4__ -D__APCS_32__ -D__LITTLEENDIAN__ -D__ARMEL__ -D_MUDFLAPTH

LINT_C_INCLUDES = \
	$(CC_IFLAG)C:/lint.PosixBasedOS001 \
	$(CC_IFLAG)$(PosixBasedOS001_TARGET)/usr/include \

LINT_CPP_INCLUDES = \
	$(CC_IFLAG)C:/lint.PosixBasedOS001 \
	$(CC_IFLAG)$(PosixBasedOS001_TARGET)/usr/include \
	$(CC_IFLAG)$(PosixBasedOS001_HOST)/usr/lib/gcc/arm-unknown-nto-PosixBasedOS001.5.0/4.4.2/include \
	$(CC_IFLAG)$(PosixBasedOS001_TARGET)/usr/include/cpp/c \
	$(CC_IFLAG)$(PosixBasedOS001_TARGET)/usr/include/cpp \

LINT_SOURCES := $(shell find .. -iname "*.c" -or -iname "*.cc" -or -iname "*.cpp")
LINT_IMPLIED_DIRS := $(addprefix $(CC_IFLAG), $(sort $(dir $(shell find .. -iname "*.c" -or -iname "*.cc" -or -iname "*.cpp"))))

# Metric Rule
metrics:
	@echo --------------------
	@echo Start metrics on $(COMPONENT_NAME)
	@echo --------------------
	-@mkdir -p $(PRJ_ROOT)/ana/metrics
	-@$(CODE_COUNT) -detailed -progress -out $(PRJ_ROOT)/ana/metrics/metrics_$(COMPONENT_NAME).csv ..

# Static Analysis Rule
lint:
	@echo --------------------
	@echo Performing Lint on $(COMPONENT_NAME)
	@echo --------------------
	@echo  includes the following files:
	@echo $(LINT_SOURCES)
	@echo --------------------
	-@c:/lint/$(LINT) $(PRJ_ROOT)cfg/options.lnt C:/lint.PosixBasedOS001/co-gcc.lnt $(CC_DEFS) $(LINT_C_INCLUDES) $(INCLUDES) $(LINT_IMPLIED_DIRS) $(filter %.c ,$(LINT_SOURCES))
	-@c:/lint/$(LINT) $(PRJ_ROOT)cfg/options.lnt C:/lint.PosixBasedOS001/co-gcc.lnt $(CPP_DEFS) $(LINT_CPP_INCLUDES) $(INCLUDES) $(LINT_IMPLIED_DIRS) $(filter %.cpp %.cc %.cxx,$(LINT_SOURCES) )
