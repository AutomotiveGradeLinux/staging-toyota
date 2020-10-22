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
/**
 * @file
 * @brief This is local header file of  Device Detection service.
 */

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_SystemServicesIf
/// \brief    This file supports the Device Detection service.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _SS_DEVICEDETECTION_SERVER_LOCAL_H_  // NOLINT (build/header_guard)
#define _SS_DEVICEDETECTION_SERVER_LOCAL_H_


// Structure for SS_DEV_REMOUNT_PARTITION
typedef struct _RemountPatitionInfo {
  unsigned long  mountflags;  // NOLINT (runtime/int)
  CHAR    deviceMountpath[FILEPATH_SIZE];  // Mount path
} SS_RemountPatitionInfo;

#endif /* _SS_DEVICEDETECTION_SERVER_LOCAL_H_ */  // NOLINT (build/header_guard)
