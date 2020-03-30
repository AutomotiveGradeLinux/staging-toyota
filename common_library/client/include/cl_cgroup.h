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

#ifndef _cl_cgroup_h_  // NOLINT(build/header_guard)
#define _cl_cgroup_h_  // NOLINT(build/header_guard)

typedef enum {
  CL_CGROUP_MEMORY,
  CL_CGROUP_CPU,
} cl_cgroup_t;

int cl_cgroup_make(cl_cgroup_t cgroup, const char *cgroup_name);  // NOLINT(readability/nolint)
int cl_cgroup_remove(cl_cgroup_t cgroup, const char *cgroup_name);  // NOLINT(readability/nolint)
int cl_cgroup_exist(cl_cgroup_t cgroup, const char *cgroup_name);  // NOLINT(readability/nolint)
int cl_cgroup_open(cl_cgroup_t cgroup, const char *cgroup_name,  // NOLINT(readability/nolint)
                   const char *controler, int flags);
int cl_cgroup_set_string(cl_cgroup_t cgroup, const char *cgroup_name,  // NOLINT(readability/nolint)
                         const char *controler, const char *str);
int cl_cgroup_set_num(cl_cgroup_t cgroup, const char *cgroup_name,  // NOLINT(readability/nolint)
                      const char *controler, int64_t value);

#endif  // ifndef _cl_cgroup_h_  // NOLINT(build/header_guard)
