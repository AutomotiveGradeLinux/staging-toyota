#!/bin/bash

#
# Copyright (c) 2019 TOYOTA MOTOR CORPORATION
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
 
cp tskm_data_init.cpp.in tskm_data_init.cpp.tmp

LF=$(printf '\\\012_')
LF=${LF%_}

for files in `ls obj/*.os`
do
  serviceName=`nm -C $files | grep " T " | grep -o "tskm_[0-9a-zA-Z_]\+_data_init" | sed -e "s/^tskm_//" | sed -e "s/_data_init\$//"`
  
  funcname="tskm_"$serviceName"_data_init"
  
  echo $serviceName $funcname
  sed -i "s/@DATA_INIT_PROT@/EFrameworkunifiedStatus $funcname(T_SS_SM_START_DataStructType* );$LF@DATA_INIT_PROT@/g" tskm_data_init.cpp.tmp
  sed -i "s/@DATA_INIT_TABLE@/{\"$serviceName\", $funcname},$LF  @DATA_INIT_TABLE@/g" tskm_data_init.cpp.tmp
  
done

sed -i "s/@DATA_INIT_PROT@//g" tskm_data_init.cpp.tmp
sed "s/@DATA_INIT_TABLE@/{NULL , NULL},/g" tskm_data_init.cpp.tmp > tskm_data_init.cpp

rm tskm_data_init.cpp.tmp






