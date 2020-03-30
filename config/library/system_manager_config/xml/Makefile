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

#HOST TOOLS
HOST_CPP = cpp
HOST_LAUNCH_XML2CFG = ./launch_xml2cfg.sh

CONF_DIR = $(SDKTARGETSYSROOT)/usr/agl/conf
CONF_THREAD_H = $(SDKTARGETSYSROOT)/usr/agl/include/agl_thread.h

LAUNCH_XMLFILES = $(shell cd $(CONF_DIR)/sm_launch; ls *.xml)
LAUNCH_CFGFILES = $(LAUNCH_XMLFILES:%.xml=%.cfg)

INST_CONFDIR = $(SDKTARGETSYSROOT)/usr/agl/conf/systemmanager

CLEAN_FILES = system_launcher_body.xml $(LAUNCH_CFGFILES)

VPATH = $(CONF_DIR)/sm_launch

install:install-pre install-cfg

install-pre:
	install -d -m 775  $(DESTDIR)/usr/agl/conf;\
	install -d -m 775  $(DESTDIR)/usr/agl/conf/sm_launch;\
	install -d -m 775  $(DESTDIR)/usr/agl/conf/systemmanager;\
	install -m 644 -t  $(DESTDIR)/usr/agl/conf/sm_launch sm_launch_agl.xml 

install-cfg:$(LAUNCH_CFGFILES)
	set -e                                 ;\
	install -d -m 775  $(INST_CONFDIR)     ;\
	for cfgfile in $(LAUNCH_CFGFILES);do   \
		install -m 644 -t $(INST_CONFDIR) $$cfgfile;\
	done                                  ;\

#Create CFG file
# %.cfg:%.xml  # defined in agl.mk

PHONY:install-cfg

include ../../../../system_service.mk
