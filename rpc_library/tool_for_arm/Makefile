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

all test: apidef.tab.h

clean:
	rm -rf $(CLEAN_FILES)

YACC := bison
YFLAGS := -d -t

VPATH += ../tool
CPPFLAGS += -I../tool
OUTPUTDIR += ../tool

apidef.tab.h apidef.tab.c:	apidef.y
	cd ../tool; $(YACC) $(YFLAGS) $<

CLEAN_FILES := ../tool/apidef.tab.h ../tool/apidef.tab.c

CLEAN_TARGET := clean-test clean-all clean-all-test \
clean-self clean-self-test clean-all-sub clean-self-sub #comment out for make clean errorclean-sub 

EMPTY_TARGET := install-test build-lib build-prog build-shlib build-arlib \
build-test-mock-lib build-test-mock-arlib build-test-mock-shlib build-cfg \
install install-header install-test-mock-header install-test-casename install-lib \
install-shlib install-arlib install-prog install-data install-cfg install-preload


.PHONY: all install test clean $(CLEAN_TARGET) $(EMPTY_TARGET)

$(CLEAN_TARGET): clean 

$(EMPTY_TARGET):


include ../../other_service.mk
