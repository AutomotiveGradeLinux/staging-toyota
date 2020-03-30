#############################################################
#
# Common Makefile for native_service
# Copyright (C) 2017-2019 TOYOTA MOTOR CORPORATION
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
#############################################################
 
CURRENT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
 
#############################################################
# COMPONENT_NAME must not be blank and be named snake_case
 
COMPONENT_NAME := native_service
 
#############################################################
 
 
 
#############################################################
# You can add several flags and libraries.
# When you add -I or -L path, DO NOT USE relative path.
# Instead, use $(CURRENT_DIR) variable
# that indicates the path this .mk file is stored.
 
COMPONENT_CFLAGS :=
COMPONENT_CXXFLAGS :=
COMPONENT_LDLIBS :=
COMPONENT_LDFLAGS :=
 
##############################################################
 
include $(SDKTARGETSYSROOT)/usr/agl/share/agl.mk
