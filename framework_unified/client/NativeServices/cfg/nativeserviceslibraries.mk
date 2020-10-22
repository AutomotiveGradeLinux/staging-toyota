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

# TODO make this more functionally oriented.
#  That is to make them more like the types ... so client libraries, etc.

#NATIVE_SERVICES_DYNAMIC_LIBS = \
#	NS_FrameworkCore$(DEBUG_EXT) \
#	NS_Logger$(DEBUG_EXT) \
#	NS_Timer$(DEBUG_EXT)\
#	NS_MessageCenter$(DEBUG_EXT) \
#	NS_MessageQueue$(DEBUG_EXT) \
#	NS_SharedMemIf$(DEBUG_EXT) \
#	NS_NPServiceIf$(DEBUG_EXT) \
#	NS_UtilityCenter$(DEBUG_EXT) \
#	NS_ConfigParser$(DEBUG_EXT) \
#	NS__CWORD77__ServiceIf$(DEBUG_EXT) \

# For improving startup performance, all libraries are merged into one
# Other modules can still use individual libraries if required. Other NS libraries are also available.
#NATIVE_SERVICES_DYNAMIC_LIBS = \
#	NS_Storage$(DEBUG_EXT) \
#	sqlite3 \
#	z \
#	NS_FrameworkUnified$(DEBUG_EXT) \

NATIVE_SERVICES_DYNAMIC_LIBS = \
	NS_FrameworkUnified$(DEBUG_EXT) \

NATIVE_SERVICES_IMAGE_STORE_DYNAMIC_LIBS = \
	sqlite3 \
	z \
	NS_Storage$(DEBUG_EXT) \
	NS_ImageStorage$(DEBUG_EXT) \

NATIVE_SERVICES_DYNAMIC_LIBS_CORE = \
	NS_CoreUnified$(DEBUG_EXT) \

TEST_AUTOMATION_FRAMEWORK_STATIC_LIBS = \
	NS_TestAutomationFramework$(DEBUG_EXT) \

