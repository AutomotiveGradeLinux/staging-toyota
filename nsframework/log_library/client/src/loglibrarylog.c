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

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <native_service/loglibrarylog_if.h>
#include "loglibrarylog_internal.h"

int
LOGLIBRARYLOG_Freeze(void) {
  int fd;
  int *pmap;
  int ret;

  fd = shm_open(FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXO);
  if (fd == -1) {
    fprintf(stderr, "[%s:L%d]shm_open(%s) error(%d)\n", __FUNCTION__, __LINE__,
            FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME, errno);
    return -1;
  }

  ret = ftruncate(fd, sizeof(int));
  if (ret == 0) {
    fprintf(stderr, "[%s:L%d]ftruncate() error(%d)\n", __FUNCTION__, __LINE__, ret);
    close(fd);
    return -1;
  }

  pmap = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  if (pmap == MAP_FAILED) {
    fprintf(stderr, "[%s:L%d]mmap() error(%d)\n", __FUNCTION__, __LINE__, errno);
    return -1;
  }
  *pmap = FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE;

  if (munmap(pmap, sizeof(int)) != 0) {
    fprintf(stderr, "[%s:L%d]munmap() error(%d)\n", __FUNCTION__, __LINE__, errno);
  }

  return 0;
}
