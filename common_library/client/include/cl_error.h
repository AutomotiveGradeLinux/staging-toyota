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

#ifndef _cl_error_h_  // NOLINT(build/header_guard)
#define _cl_error_h_  // NOLINT(build/header_guard)

#include <stdio.h>
#include <unistd.h>
#include <asm/unistd.h>

static inline int gettid(void) {  // NOLINT(readability/nolint)
  return (int)syscall(__NR_gettid);  // NOLINT(readability/casting)
}

#define _num_to_str(num) #num
#define num_to_str(num) _num_to_str(num)
#define CL_PERROR(msg) \
  perror(__FILE__ ":" num_to_str(__LINE__)":"msg)

#define CL_ERR_PRINT(fmt, args...) \
  fprintf(stderr, "[ERR]%d %s:%d:"fmt"\n", gettid(), __func__, __LINE__, ##args)

#if defined(CL_DBG_ENABLE)
#define CL_DBG_PRINT(fmt, args...) \
    fprintf(stdout, "[DBG]%d %s:%d:"fmt"\n", gettid(), __func__, __LINE__, ##args)
#else
#define CL_DBG_PRINT(fmt, args...)
#endif


#endif  // #ifndef _cl_error_h_  // NOLINT(build/header_guard)
