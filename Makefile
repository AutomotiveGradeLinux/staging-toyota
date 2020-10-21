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

######### installed program/file ################
HEADER   = hal_api/update_hal.h

.PHONY:all

######### make targets ##########################
all:
	@echo $@ done.

######### make cleans ###########################
clean:
	@echo $@ done.

######### make installs #########################
install:
	install -d -m 0755 $(DESTDIR)/usr/include
	install -m 0644 $(HEADER) $(DESTDIR)/usr/include
	@echo $@ done.
