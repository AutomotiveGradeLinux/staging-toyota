/*
 * @copyright Copyright(c) 2018-2020 TOYOTA MOTOR CORPORATION.
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
#include "nv_hal.h"
#include <errno.h>
#include <fcntl.h>
#include <aglpath.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nv_hal_nvhallog.h"

/**
 * Media type difine.
 */
const char *kMediaType[NVHALMEDIA_MAX] = {
  "/tmp/bkup/",
  "/ramd/BS/ns/backup_manager/rwdata/",
  "/nv/BS/ns/backup_manager/rwdata/",
};

const int kMaxPath = 127;    // Max length of path

/**
 *  Initialize Nv hal
 */
EFrameworkunifiedStatus InitNv(void) {
  return eFrameworkunifiedStatusOK;
}

/**
 * Get data size.
 */
EFrameworkunifiedStatus GetSizeNv(enum NvHalMedia media, const char *filename, uint32_t *size) {
  if ((media < NVHALMEDIA_CACHEDRAM) || (media > NVHALMEDIA_NAND)) {  // Argument range checking
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid media type:%d\n", media);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == filename) {  // NULL checking of arguments
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "filename is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == size) {  // NULL checking of arguments
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "size is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  char path[kMaxPath];
  memset(path, '\0', kMaxPath);
  snprintf(path, kMaxPath, "%s%s", kMediaType[media], filename);
  // Geting the file size
  struct stat file_stat;
  if (0 > lstat(path, &file_stat)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lstat() error, errno=%d\n", errno);
    return eFrameworkunifiedStatusFileLoadError;
  }
  *size = file_stat.st_size;
  return eFrameworkunifiedStatusOK;
}



/**
 * Reading data from memory device.
 */
EFrameworkunifiedStatus ReadNv(enum NvHalMedia media, const char *filename, uint8_t *buffer, uint32_t size) {
  if ((media < NVHALMEDIA_CACHEDRAM) || (media > NVHALMEDIA_NAND)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid media type:%d\n", media);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == filename) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "filename is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == buffer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "buffer is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == size) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid size:%d\n", size);
    return eFrameworkunifiedStatusInvldParam;
  }
  char path[kMaxPath];
  memset(path, '\0', kMaxPath);
  snprintf(path, kMaxPath, "%s%s", kMediaType[media], filename);
  struct stat file_stat;
  if (0 > lstat(path, &file_stat)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lstat() error, errno=%d\n", errno);
    return eFrameworkunifiedStatusFail;
  }
  if (file_stat.st_size != size) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "size error:%ld != %d\n", file_stat.st_size, size);
    return eFrameworkunifiedStatusFail;
  }
  int fd = open(path, O_RDONLY);
  if (-1 == fd) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open(%s) error, errno=%d\n", path, errno);
    return eFrameworkunifiedStatusFail;
  } else {
    size_t tot_read;
    // Read data by support EINTR
    for (tot_read = 0; tot_read < static_cast<size_t>(size);) {
      ssize_t read_size = pread(fd, &buffer[tot_read], static_cast<size_t>(size) - tot_read, tot_read);
      if (-1 == read_size) {
        if (errno == EINTR) {
          continue;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pread() error, errno=%d\n", errno);
          if (0 != close(fd)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "close() error, errno=%d\n", errno);
          }
          return eFrameworkunifiedStatusFail;
        }
      }
      tot_read += read_size;
    }
    if (0 != close(fd)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "close() error, errno=%d\n", errno);
      return eFrameworkunifiedStatusFail;
    }
  }
  return eFrameworkunifiedStatusOK;
}

/**
 * Writing data to memory device.
 */
EFrameworkunifiedStatus WriteNv(enum NvHalMedia media, const char *filename, uint8_t *buffer, uint32_t size) {
  if ((media < NVHALMEDIA_CACHEDRAM) || (media > NVHALMEDIA_NAND)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid media type:%d\n", media);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == filename) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "filename is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == buffer) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "buffer is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (0 == size) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid size:%d\n", size);
    return eFrameworkunifiedStatusInvldParam;
  }
  char path[kMaxPath];
  memset(path, '\0', kMaxPath);
  snprintf(path, kMaxPath, "%s%s", kMediaType[media], filename);
  struct stat file_stat;
  // Check file exists or not, mkdir first if file no exists.
  if (0 > lstat(path, &file_stat)) {
    char *dir_point = path;
    char dir_buff[kMaxPath];
    dir_point++;
    while ((dir_point = strchr(dir_point, '/'))) {
      memset(dir_buff, '\0', kMaxPath);
      memcpy(dir_buff, path, dir_point - path);
      if (0 > mkdir(dir_buff, 0770)) {
        if (EEXIST != errno) {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "mkdir() error, errno=%d\n", errno);
          return eFrameworkunifiedStatusFail;
        } else {
          dir_point++;
          continue;
        }
      }
      dir_point++;
    }
  }
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0660);
  if (-1 == fd) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open(%s) error, errno=%d\n", path, errno);
    return eFrameworkunifiedStatusFail;
  } else {
    size_t tot_written;
    // Write data by support EINTR
    for (tot_written = 0; tot_written < static_cast<size_t>(size);) {
      ssize_t write_size = pwrite(fd, &buffer[tot_written], static_cast<size_t>(size) - tot_written, tot_written);
      if (0 >= write_size) {
        if (-1 == write_size && errno == EINTR) {
          continue;
        } else {
          FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "pwrite() error, errno=%d\n", errno);
          if (0 != close(fd)) {
            FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "close() error, errno=%d\n", errno);
          }
          return eFrameworkunifiedStatusFail;
        }
      }
      tot_written += write_size;
    }
    if (0 != close(fd)) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "close() error, errno=%d\n", errno);
      return eFrameworkunifiedStatusFail;
    }
  }
  return eFrameworkunifiedStatusOK;
}

/**
 * Delete data.
 */
EFrameworkunifiedStatus DeleteNv(enum NvHalMedia media, const char *filename) {
  if ((media < NVHALMEDIA_CACHEDRAM) || (media > NVHALMEDIA_NAND)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "invalid media type:%d\n", media);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (NULL == filename) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "filename is NULL.\n");
    return eFrameworkunifiedStatusInvldParam;
  }

  char path[kMaxPath];
  memset(path, '\0', kMaxPath);
  snprintf(path, kMaxPath, "%s%s", kMediaType[media], filename);
  struct stat file_stat;

  if (0 > lstat(path, &file_stat)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "lstat() error, errno=%d\n", errno);
    return eFrameworkunifiedStatusFail;
  }

  if (0 != unlink(path)) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "unlink() error, errno=%d\n", errno);
    return eFrameworkunifiedStatusFail;
  }

  return eFrameworkunifiedStatusOK;
}
