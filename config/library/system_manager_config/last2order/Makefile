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

# ==== Customization ================
#Directory path located XMLs for conversion
CONFXML_DIR_PATH ?= $(SDKTARGETSYSROOT)/usr/agl/conf/sm_last2order

#path to order file
ORDER_DEF_PATH ?= $(SDKTARGETSYSROOT)/usr/agl/include/agl_wakeup_order.h

#path to be installed CFO file
CONF_INSTALL_DIR ?= $(SDKTARGETSYSROOT)/usr/agl/conf/systemmanager

# ==================================

HOST_CPP = cpp
MKL_LAST2ORDER = mklast2ordertbl.pl

#path located last2order tools
TOOLPATH = ./tool

VPATH = $(CONFXML_DIR_PATH) $(TOOLPATH)

XMLFILES = $(shell cd $(CONFXML_DIR_PATH); ls *.xml)
XMLTMPFILES = $(XMLFILES:%.xml=%.xml.pped)
CFOFILES = $(XMLFILES:%.xml=%.cfo)

_all:$(CFOFILES)

install:install-pre install-cfo 

install-pre:
	install -d -m 775  $(DESTDIR)/usr/agl/conf/sm_last2order;\
	install -m 644 -t  $(DESTDIR)/usr/agl/conf/systemmanager sm_last2order.cfo 

install-cfo:_all
	set -e ;\
	install -d -m 775 $(CONF_INSTALL_DIR) ;\
	for cfofile in $(CFOFILES); do \
		install -m 644 -t $(CONF_INSTALL_DIR) $$cfofile; \
	done

%.cfo:%.xml.pped $(MKL_LAST2ORDER)
	ln -fs $(TOOLPATH)/$(MKL_LAST2ORDER) 
	./$(MKL_LAST2ORDER) $< $@ 

%.xml.pped:%.xml 
	$(HOST_CPP) -P -include $(ORDER_DEF_PATH) $< > $@

clean:clean_local
clean-sub:clean_local

clean_local:
	rm -f *.cfo *.pped *.pl

.PHONY:install-cfo _all

include ../../../../system_service.mk
