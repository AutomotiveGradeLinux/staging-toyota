/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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

#ifndef TASK_MANAGER_INCLUDE_SHARE_TSKM_TYPE_LOCAL_H_
#define TASK_MANAGER_INCLUDE_SHARE_TSKM_TYPE_LOCAL_H_

#define NTFY_SS_TaskManager_Availability  SS_TASK_MANAGER"/Availability"

//*******************************************************
//      Legacy types:
//       Definition is retained because it is referenced by another module
//******************************************************

// Shared LOG with version upgrade
#define STATEMGR__CWORD69_LOG_SIZE 176   /* STATEMGR__CWORD69_LOG  */
typedef struct {
  uint8_t old_Ver[8];
  uint8_t new_Ver[8];
} STATEMGR__CWORD69_LOG_VERSION_T;

typedef struct {
  STATEMGR__CWORD69_LOG_VERSION_T ver;
  uint8_t status;
  uint8_t dummy[3];
} STATEMGR__CWORD69_LOG_VUPINFO_T;

typedef struct {
  uint32_t seqnum; /* Set value (for chequence check) */
  uint32_t normalnum; /* Set value (for route check) */
  uint32_t errornum; /* Set value (for NG check)  */
} STATEMGR__CWORD69_LOG_PROC;

typedef struct {
  STATEMGR__CWORD69_LOG_VUPINFO_T VerInfo[7]; /* Store version info */
  STATEMGR__CWORD69_LOG_PROC proc[3]; /* Store logs for analysis */
} STATEMGR__CWORD69_LOG;

#endif  // TASK_MANAGER_INCLUDE_SHARE_TSKM_TYPE_LOCAL_H_

