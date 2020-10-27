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

##
##  Makefile - Makefile for TimeZone file
##

ZONEINFDIR  = $(DESTDIR)/usr/share/zoneinfo/
ZONEINFLNK  = /etc/
ZONEINFDIR2 = $(DESTDIR)/etc/
ZONEINFLNK2 = /tmp/bs/clock/
ZONEINFFILE= localtime
DSTDIR = $(ZONEINFDIR)DST
CLEAN_DIRS = $(ZONEINFDIR)

LDFLAGS += -Wl,--no-undefined
CPPFLAGS += -Werror=implicit-function-declaration
CPPFLAGS += -Werror=format-security

CPPFLAGS += -Wconversion
CPPFLAGS += -Wint-to-pointer-cast
CPPFLAGS += -Wpointer-arith
CPPFLAGS += -Wformat

install-data:
	rm -rf $(ZONEINFDIR) ;\
	mkdir -p $(DSTDIR) ;\
	mkdir -p $(ZONEINFDIR2) ;

##	/usr/sbin/zic -d $(ZONEINFDIR) zoneinfo_base;
	zic -d $(ZONEINFDIR) zoneinfo_base;

	ln -f -s $(ZONEINFLNK)$(ZONEINFFILE) $(ZONEINFDIR)$(ZONEINFFILE); \
	ln -f -s $(ZONEINFLNK2)$(ZONEINFFILE) $(ZONEINFDIR2)$(ZONEINFFILE)

include ../../stub.mk
