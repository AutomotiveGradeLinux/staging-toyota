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

#HOST TOOLS
HOST_CPP = cpp
HOST_ORDER_XML2CFG = ./launch_order_xml2cfg.pl

CONF_DIR = $(SDKTARGETSYSROOT)/usr/agl/conf
CONF_WON_H = $(SDKTARGETSYSROOT)/usr/agl/include/agl_wakeup_order.h

LAUNCH_XMLFILES = $(shell cd $(CONF_DIR)/sm_launch; ls *.xml)
ORDER_XMLFILES = $(LAUNCH_XMLFILES:%.xml=%.order)
ORDER_CFGFILES = $(ORDER_XMLFILES:%.order=%.order.cfg)

INST_CONFDIR = $(SDKTARGETSYSROOT)/usr/agl/conf/systemmanager

CLEAN_FILES = launch_order_body.xml $(ORDER_CFGFILES)

VPATH  = $(CONF_DIR)/sm_launch
VPATH += ../xml

install:install-pre install-cfg

install-pre:
	install -m 644 -t  $(DESTDIR)/usr/agl/conf/sm_launch system_launcher_main.xml 

install-cfg:$(ORDER_CFGFILES)
	set -e                                 ;\
	install -d -m 775  $(INST_CONFDIR)     ;\
	for cfgfile in $(ORDER_CFGFILES);do   \
		install -m 644 -t $(INST_CONFDIR) $$cfgfile;\
	done                                  ;\

#Create CFG file
%.order.cfg:%.order %.cfg
	echo $@
	set -e; \
	$(HOST_CPP) $(CPPFLAGS) -P -include $(CONF_WON_H) $< > launch_in.order; \
	$(HOST_ORDER_XML2CFG) launch_in.order $(filter %.cfg,$+) > $@ ; \
	rm -fr launch_in.order;

%.order:
	echo $@
	set -e; \
	if [ ! -e $@ ] ; then \
	  touch $@     ;      \
	fi

PHONY:install-cfg

include ../../../../system_service.mk
