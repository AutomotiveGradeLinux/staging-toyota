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

SVCONF_DIR=$(DESTDIR)/usr/agl/conf/tm_launch
THREADCONF_DIR=$(DESTDIR)/usr/agl/conf/thread
WAKESHUTCONF_DIR=./conf

VPATH = ./src ./include/$(COMPONENT_NAME)

INST_SHLIBS = libtskmcfg
INST_HEADERS = tskm_xml_data.h  tskm_svcid.h task_manager_libtskmcfg.h
libtskmcfg_SRCS = tskm_xml_data.cpp


CPPFLAGS = -I./ -I./include -I./../../include-share -I./../../server/include 
LDFLAGS += -Wl,--no-as-needed
LDLIBS += -Wl,-Bdynamic -lNS_FrameworkUnified


#============ XML PARSE ============
XMLOUTFILE= tskm_auto_build.h

CLEAN_FILES+=$(XMLOUTFILE) $(PARSEXMLCMD) $(CONFIG_FILES) top.xml

#TOOLS
HOST_CPP=cpp
XMLCHECKER=xmllint
XMLCHECKEROPT=--noout --valid
PARSEXMLCMD=parsexml
HOSTCC=gcc
XMLFILETOP=tskm_cfg.xml

install:install-pre install-pre-header
build-lib:install-pre install-pre-header
install-lib:install-pre install-pre-header
install-pre:
	install -d -m 775  $(DESTDIR)/usr/agl/conf/tm_launch;\
	install -d -m 775  $(DESTDIR)/usr/agl/conf/thread;\
	install -m 644 -t  $(DESTDIR)/usr/agl/conf/tm_launch $(WAKESHUTCONF_DIR)/tskm_launch.xml;\
	install -m 644 -t  $(DESTDIR)/usr/agl/conf/thread $(SDKTARGETSYSROOT)/usr/agl/include/agl_thread.h

CONFIG_FILES:=tskm_wakeup.xml tskm_shutdown.xml tskm_wakeup_vup.xml tskm_shutdown_vup.xml tskm_launch.xml tskm_svcid.h agl_thread.h

#install-header:$(XMLOUTFILE) 
install-pre-header:$(XMLOUTFILE) 

#TOPFILE
top.xml:$(XMLFILETOP)
	ln -fs $< $@

#CONFIG RULES
tskm_wakeup.xml:$(WAKESHUTCONF_DIR)/tskm_wakeup.xml
	ln -fs $< $@

tskm_shutdown.xml:$(WAKESHUTCONF_DIR)/tskm_shutdown.xml
	ln -fs $< $@

tskm_wakeup_vup.xml:$(WAKESHUTCONF_DIR)/tskm_wakeup_vup.xml
	ln -fs $< $@

tskm_shutdown_vup.xml:$(WAKESHUTCONF_DIR)/tskm_shutdown_vup.xml
	ln -fs $< $@

#tskm_svcid.h:$(SVCONF_DIR)/tskm_svcid.h
#	ln -fs $< $@

agl_thread.h:$(THREADCONF_DIR)/agl_thread.h
	ln -fs $< $@


tskm_launch.xml:$(SVCONF_DIR)/tskm_launch.xml agl_thread.h
	$(HOST_CPP) -P -include agl_thread.h $< > $@
#tskm_launch.xml:$(SVCONF_DIR)/tskm_launch.xml $(SDKTARGETSYSROOT)/usr/agl/include/agl_thread.h
#	$(HOST_CPP) -P -include agl_thread.h $< > $@

$(XMLOUTFILE):top.xml $(PARSEXMLCMD) $(CONFIG_FILES) 
	$(XMLCHECKER) $(XMLCHECKEROPT) $<
	./$(PARSEXMLCMD) $<

$(PARSEXMLCMD):$(PARSEXMLCMD).c
	$(HOSTCC) -o $@ $< -lexpat

include ../../../system_service.mk

