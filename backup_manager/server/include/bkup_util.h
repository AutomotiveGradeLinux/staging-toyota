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

#ifndef BACKUP_MANAGER_SERVER_INCLUDE_BKUP_UTIL_H_
#define BACKUP_MANAGER_SERVER_INCLUDE_BKUP_UTIL_H_

#include <sys/mman.h>
#include <ctime>
#include <cstring>

static inline size_t BkupStrlcpy(char *dst, const char *src, size_t siz) {
  size_t ret = strlen(src);
  if (dst == NULL) {
    return -1;
  }
  if (src == NULL) {
    return -1;
  }
  if (siz) {  // LCOV_EXCL_BR_LINE 6:double check
    size_t len = (ret >= siz) ? siz - 1 : ret;  // LCOV_EXCL_BR_LINE 11:unexpected branch
    memcpy(dst, src, len);
    dst[len] = '\0';
  }
  return ret;
}

static inline char *BkupAnonMmap(size_t size) {
  char *map = static_cast<char *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  return map;
}

static inline uint64_t BkupTimerReal(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != -1) {  // LCOV_EXCL_BR_LINE 5:clock_gettime's error case.
    return (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
  }
  return -1;
}

#endif  // BACKUP_MANAGER_SERVER_INCLUDE_BKUP_UTIL_H_
