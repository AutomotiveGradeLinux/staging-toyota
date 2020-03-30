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

all: rpc_apidef

clean:
	rm -rf $(CLEAN_FILES)

YACC := bison
YFLAGS := -d -t
LEX := flex
LFLAGS := -d

CPPFLAGS := -I../library/include -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h
CFLAGS := -Wall -Wno-unused-function -Wno-unused-variable -include $(SDKTARGETSYSROOT)/usr/agl/include/agl_types_obsoluted.h

rpc_apidef:	apidef.tab.o lex.yy.o apidef.o
	$(CXX) $(CPPFLAGS) -o $@ $^

apidef.tab.o:	apidef.tab.h apidef.tab.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^

lex.yy.o:	lex.yy.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^

apidef.o:	apidef.cc
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $^

apidef.tab.h apidef.tab.c:	apidef.y
	$(YACC) $(YFLAGS) $<

lex.yy.c:	apidef.l apidef.tab.h
	$(LEX) $(LFLAGS) $<

CLEAN_FILES := rpc_apidef *.o apidef.tab.h apidef.tab.c lex.yy.c

CLEAN_TARGET := clean-all clean-all-sub clean-sub
INSTALL_TARGET := install-header
EMPTY_TARGET := install-prog install-data install-lib install-arlib install-shlib
.PHONY: all install clean $(CLEAN_TARGET) $(INSTALL_TARGET) $(EMPTY_TARGET)

$(CLEAN_TARGET): clean

$(INSTALL_TARGET): install

$(EMPTY_TARGET):

include ../../other_service.mk

