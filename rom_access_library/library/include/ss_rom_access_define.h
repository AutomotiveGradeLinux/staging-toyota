/*
 * @copyright Copyright (c) 2017-2019 TOYOTA MOTOR CORPORATION.
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


#ifndef ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_ROM_ACCESS_DEFINE_H_
#define ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_ROM_ACCESS_DEFINE_H_

#include <errno.h>

#define ROM_ACCESS_ASERT(x) \
  if (!(x)) { \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_ASSERT"); \
  }

#define ROM_ACCESS_ERRNO(x) \
  if (!(x)) { \
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "SS_ASSERT %d:%s", errno, strerror(errno)); \
  }

#define ROM_ACCESS_STATIC_ASERT(expr) \
  { \
    char STATIC_ASSERTION_FAILED[(expr) ? 1 : -1]; \
    (void)STATIC_ASSERTION_FAILED; \
  }
#endif  // ROM_ACCESS_LIBRARY_LIBRARY_INCLUDE_SS_ROM_ACCESS_DEFINE_H_
