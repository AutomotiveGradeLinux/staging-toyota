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

#ifndef _cl_process_internal_h_  // NOLINT(build/header_guard)
#define _cl_process_internal_h_  // NOLINT(build/header_guard)

typedef struct {
  char name[20];
  uid_t uid;
  gid_t gid;
  int sched_policy;
  int sched_priority;
  int create_group;
  int stack_size;
  int cpu_assign;
  int hold_fds[CL_PROCESSS_ATTR_HOLD_FDS_NUM];
  int disable_close_fds;
  char cgroup_name[64];
} CL_ProcessAttrInternal_t;

typedef struct {
  char name[20];
} CL_ThreadAttrInternal_t;

typedef struct {
  int rt_runtime_us;
  int cfs_quota_us;
  int cpu_shares;
  int memory_limit;
  int usage_in_bytes;
  int event_fd;
} CL_ProcessCreateCgroupAttrInternal_t;

#define CL_PROCESS_NAME_ENV "CL_PROCESS_NAME"

#define CL_PROCESS_EXIT_INTERNAL 0x80

#define CL_PROCESS_CGROUP_NAME_FORMAT "CGRP%05d"

#endif  // #ifndef _cl_process_internal_h_  // NOLINT(build/header_guard)
