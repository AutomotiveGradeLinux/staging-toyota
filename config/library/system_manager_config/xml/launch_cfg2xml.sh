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
 
export infile=$1
export g_group=1

if ! [[ "$infile" =~ .+\.cfg ]] ;then
  echo "$infile is not *.cfg"
  exit 1
elif ! [ -e $infile ];then
  echo "$infile not found"
  exit 1
fi

#GROUP extraction
cat ${infile} | while read line 
do
  if ! [[ "${line}" =~ ^[#\[] ]]; then    #Remove first #[
    echo $line | sed 's/Launch[0-9]\+=//g' | cut -d '|' -f1-4
  fi
done | uniq | while read line
do
  echo "<!-- group_id=$g_group -->"
  echo "<group name=\"`echo $line | cut -d '|' -f1`\" wait_time=\"`echo $line | cut -d '|' -f3`\" trigger=\"`echo $line | cut -d '|' -f4`\" >"
  
  cat ${infile} | while read line 
  do
    if ! [[ "${line}" =~ ^[#\[] ]]; then    #Remove first #[
      l_group=`echo $line | sed 's/Launch[0-9]\+=//g' | cut -d '|' -f2`

      #Extract only LAUNCH applicable to g_group
      if [ ${l_group} = ${g_group} ]; then
        echo $line | sed 's/Launch[0-9]\+=//g' | cut -d '|' -f5-
      fi
    fi
  done  | while read line
  do
    echo "  <launch "
    echo "    name=\"`echo $line | cut -d '|' -f1`\" path=\"`echo $line | cut -d '|' -f2`\" priority=\"`echo $line | cut -d '|' -f3`\""
    echo "    critical=\"`echo $line | cut -d '|' -f4`\" retry_cnt=\"`echo $line | cut -d '|' -f5`\" arguments=\"`echo $line | cut -d '|' -f6`\""
    echo "    logging_mask=\"`echo $line | cut -d '|' -f7`\" restart=\"`echo $line | cut -d '|' -f8`\" is_start_required=\"`echo $line | cut -d '|' -f9`\""
    echo "    shutdown_critical=\"`echo $line | cut -d '|' -f10`\" shutdown_wait_time=\"`echo $line | cut -d '|' -f11`\" fast_shutdown_wait_time=\"`echo $line | cut -d '|' -f12`\""
    echo "    user_name=\"`echo $line | cut -d '|' -f13`\""
    echo "  />"
  done

  echo "</group>"
  
  g_group=$(($g_group + 1))
done




