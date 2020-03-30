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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cl_cgroup.h"

#define CL_CGROOT "/sys/fs/cgroup/"

static char *cl_cgroup_base(cl_cgroup_t cgroup) {
  switch (cgroup) { // LCOV_EXCL_BR_LINE 200: internal interface,code make sure
    case CL_CGROUP_MEMORY:
      return CL_CGROOT "/memory";
    case CL_CGROUP_CPU:
      return CL_CGROOT "/cpu";
  }
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  return NULL;// LCOV_EXCL_LINE 200:internal interface,code make sure nerver run
}

static char *cl_cgroup_create_path(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler) {
  char *path = malloc(FILENAME_MAX);

  if (path == NULL) { // LCOV_EXCL_BR_LINE 5:fail safe for libc  malloc
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = ENOMEM; // LCOV_EXCL_LINE 5:fail safe for libc  malloc
    goto error; // LCOV_EXCL_LINE 5:fail safe for libc  malloc
  }

  if (cgroup != CL_CGROUP_MEMORY && cgroup != CL_CGROUP_CPU) { // LCOV_EXCL_BR_LINE 200: internal interface,code make sure
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = EINVAL; // LCOV_EXCL_LINE 200: internal interface,code make sure
    goto error; // LCOV_EXCL_LINE 200: internal interface,code make sure
  }

  if (cgroup_name == NULL) { // LCOV_EXCL_BR_LINE 200: internal interface,code make sure
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = EINVAL; // LCOV_EXCL_LINE 200: internal interface,code make sure
    goto error; // LCOV_EXCL_LINE 200: internal interface,code make sure
  }

  if (controler) {
    snprintf(path, FILENAME_MAX, "%s/%s/%s", cl_cgroup_base(cgroup), cgroup_name, controler);
  } else {
    snprintf(path, FILENAME_MAX, "%s/%s", cl_cgroup_base(cgroup), cgroup_name);
  }

  return path;

error:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  free(path); // LCOV_EXCL_LINE 200: internal interface,code make sure
  return NULL; // LCOV_EXCL_LINE 200: internal interface,code make sure
}

int cl_cgroup_make(cl_cgroup_t cgroup, const char *cgroup_name) {
  int r = -1;
  char *path = cl_cgroup_create_path(cgroup, cgroup_name, NULL);

  if (path == NULL) {  // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    goto exit;  // LCOV_EXCL_LINE 6: double check
  }

  if (mkdir(path, 0777) < 0) { // LCOV_EXCL_BR_LINE 5:fail safe for libc mkdir
    goto exit;
  }

  r = 0;

exit:
  if (path) {// LCOV_EXCL_BR_LINE 6: double check
    free(path);
  }
  return r;
}

int cl_cgroup_remove(cl_cgroup_t cgroup, const char *cgroup_name) {
  int r = -1;
  char *path = cl_cgroup_create_path(cgroup, cgroup_name, NULL);

  if (path == NULL) { // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    goto exit; // LCOV_EXCL_LINE 6: double check
  }

  if (rmdir(path) < 0) { // LCOV_EXCL_BR_LINE 5:fail safe for libc rmdir
    goto exit;
  }

  r = 0;

exit:
  if (path) { // LCOV_EXCL_BR_LINE 6: double check
    free(path);
  }
  return r;
}

int cl_cgroup_exist(cl_cgroup_t cgroup, const char *cgroup_name) {
  int r = -1;
  char *path = cl_cgroup_create_path(cgroup, cgroup_name, NULL);

  if (path == NULL) { // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    goto exit; // LCOV_EXCL_LINE 6: double check
  }

  r = access(path, F_OK);

exit:
  if (path) { // LCOV_EXCL_BR_LINE 6: double check
    free(path);
  }
  return r;
}

int cl_cgroup_open(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler, int flags) {
  int r = -1;
  char *path = cl_cgroup_create_path(cgroup, cgroup_name, controler);
  int fd = -1;

  if (path == NULL) { // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    goto exit; // LCOV_EXCL_LINE 6: double check
  }

  if (controler == NULL) { // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = EINVAL;  // LCOV_EXCL_LINE 6: double check
    goto exit;  // LCOV_EXCL_LINE 6: double check
  }

  if ((fd = open(path, flags | O_CLOEXEC)) < 0) {  // LCOV_EXCL_BR_LINE 5:fail safe for libc open
    goto exit;
  }

  r = fd;

exit:
  if (path) { // LCOV_EXCL_BR_LINE 6: double check
    free(path);
  }
  return r;
}

int cl_cgroup_set_string(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler, const char *string) {
  int r = -1;
  int fd = -1;

  if (controler == NULL || string == NULL) { // LCOV_EXCL_BR_LINE 6: double check
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = EINVAL;// LCOV_EXCL_LINE 6: double check
    goto exit;// LCOV_EXCL_LINE 6: double check
  }

  fd = cl_cgroup_open(cgroup, cgroup_name, controler, O_WRONLY);
  if (fd < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for glibc funtion open
    goto exit;
  }

  if (write(fd, string, strlen(string)) < 0) {  // LCOV_EXCL_BR_LINE 5:fail safe for libc write
    goto exit;
  }

  r = 0;

exit:
  if (fd >= 0) { // LCOV_EXCL_BR_LINE 5: fail safe for glibc funtion open
    int save_err = errno;
    close(fd);
    errno = save_err;
  }
  return r;
}

int cl_cgroup_set_num(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler, int64_t value) {
  char num_str[22];

  snprintf(num_str, sizeof(num_str), "%" PRId64, value);

  return cl_cgroup_set_string(cgroup, cgroup_name, controler, num_str);
}

int cl_cgroup_get_string(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler, char *string, size_t length) {  // LCOV_EXCL_START 8: dead code  // NOLINT (readability/nolint)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  int r = -1;
  int fd = -1;

  if (controler == NULL || string == NULL) {
    errno = EINVAL;
    goto exit;
  }

  fd = cl_cgroup_open(cgroup, cgroup_name, controler, O_RDONLY);
  if (fd < 0) {
    goto exit;
  }

  if (read(fd, string, length) < 0) {
    goto exit;
  }

  r = 0;

exit:
  if (fd >= 0) {
    int save_err = errno;
    close(fd);
    errno = save_err;
  }
  return r;
}
// LCOV_EXCL_STOP

int64_t cl_cgroup_get_num(cl_cgroup_t cgroup, const char *cgroup_name, const char *controler) {  // LCOV_EXCL_START 8: dead code  // NOLINT (readability/nolint)
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

  int r;
  char num_str[22];

  if ((r = cl_cgroup_get_string(cgroup, cgroup_name, controler, num_str, sizeof(num_str))) < 0) {
    return r;
  }

  return atoll(num_str);
}
// LCOV_EXCL_STOP
