#
# @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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

XML_PATH := ./conf/backup
RNG_FILE := backup.rng
ALL_XML := ALL_.xml
ID_HEADER := ns_backup_id.h
BACKUP_XML := backup*.xml

######### headers to be installed(*.h)   #############
INST_HEADERS := $(ID_HEADER)

######### make rule                      #############
all: $(ID_HEADER)

$(ID_HEADER): $(XML_PATH)/$(ALL_XML)
	@set -e; \
	for xml_file in `find $(XML_PATH) -name \*.xml | grep -v $(ALL_XML)`; do \
	  xmllint --noout --relaxng $(RNG_FILE) $$xml_file; \
	  ./uniqcheck.pl $$xml_file; \
	done
	./createhdr.pl $(XML_PATH)/$(ALL_XML) > $(ID_HEADER)

CLEAN_FILES := $(ID_HEADER)

install: $(XML_PATH)/$(BACKUP_XML)
	install -m 0755 -d $(DESTDIR)/usr/agl/conf/BS/ns/backup_manager/rodata
	install -m 0644 $(XML_PATH)/$(BACKUP_XML) $(DESTDIR)/usr/agl/conf/BS/ns/backup_manager/rodata/
include ../../native_service.mk
