/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemManager
/// \brief    This file provides support for default path names.
///
///////////////////////////////////////////////////////////////////////////////
#ifndef SYSTEM_MANAGER_SERVER_INCLUDE_SS_SM_DEFAULT_PATHS_H_
#define SYSTEM_MANAGER_SERVER_INCLUDE_SS_SM_DEFAULT_PATHS_H_
/*
 * Environment variables
 */
const CHAR BaseDirPathEnvVariable[]             = "BASE_DIR";
const CHAR LaunchConfigFileNameEnvVariable[]    = "APPS_CONFIG_FILENAME";
const CHAR SMConfigDataFileNameEnvVariable[]    = "SM_CONFIG_DATA_FILENAME";
const CHAR UsingVMPlayerVariable[]              = "USING_VM_PLAYER";

/*
 * Default paths to be used in case Environment variables are not set
 */
const CHAR DefaultBasePath[]                = "";
const CHAR DefaultBinaryPath[]              = "/agl/bin/";
const CHAR DefaultConfigFilesPath[]         = "/usr/agl/conf/BS/ss/system_manager/rwdata/";
const CHAR DefaultLaunchConfigFileName[]    = "sm_launch.cfg";
const CHAR DefaultSMConfigFileName[]        = "gpf_ss_sm_config_data.cfg";

#endif  // SYSTEM_MANAGER_SERVER_INCLUDE_SS_SM_DEFAULT_PATHS_H_

