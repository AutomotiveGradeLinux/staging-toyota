/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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

#include <boot_hal.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/vfs.h>

#include <cstdio>
#include <cerrno>
#include <cstring>

#include "boot_hal_boothallog.h"

#define BOOT_PARTATION "/dev/mmcblk0boot1"
#define BOOTINFO_FILEA "/repro/mmcblk0boot1-launch_a.bin"
#define BOOTINFO_FILEB "/repro/mmcblk0boot1-launch_b.bin"

#define MMCBLK0BOOT1_FORCE_RO "/sys/class/block/mmcblk0boot1/force_ro"
#define MMCBLK0BOOT1_SIZE_PATH "/sys/class/block/mmcblk0boot1/size"
#define MMCBLK0BOOT1_SIZE_DEF (0x1000000)
#define BOOTINFO_SIZE (0x20000)
#define SECTOR_SIZE (512)
#define FILEPATH_MAX (256)

#define BOOT_CMDLINE "/proc/cmdline"
#define CMDLINE_MAX_SIZE (256)
#define BOOT_ROOTFS_A "/dev/mmcblk0p1"
#define BOOT_ROOTFS_B "/dev/mmcblk0p2"
#define BOOTARGS_LST_ROOT "root="


#define BOOT_COPYBUF_SIZE (64 * 1024)

static int EnableMmcBlk(const char *path) {
  int fd;
  char enable[] = "0";

  if ((fd = open(path, O_RDWR)) < 0) {
    return -1;
  }

  if (write(fd, enable, sizeof(enable)) < 0) {
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}

static int GetMmcBlkCount(const char *path) {
  int fd;
  char block_size[16] = {0};

  if ((fd = open(path, O_RDONLY)) < 0) {
    return -1;
  }

  if (read(fd, block_size, sizeof(block_size)) < 0) {
    close(fd);
    return -1;
  }

  close(fd);
  return strtol(block_size, NULL, 10);
}

static ssize_t BootReadN(int fd, void *buffer, size_t n) {
  size_t tot_read;
  char *buf;

  buf = reinterpret_cast<char *>(buffer);
  for (tot_read = 0; tot_read < n;) {
    ssize_t num_read;
    num_read = read(fd, buf, n - tot_read);

    if (num_read == 0) {
      return tot_read;
    }
    if (num_read == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        return -1;
      }
    }
    tot_read += num_read;
    buf += num_read;
  }
  return tot_read;
}

static ssize_t BootWriteN(int fd, const void *buffer, size_t n) {
  size_t tot_written;
  const char *buf;

  buf = (const char *)buffer;
  for (tot_written = 0; tot_written < n;) {
    ssize_t num_written;
    num_written = write(fd, buf, n - tot_written);

    if (num_written <= 0) {
      if (num_written == -1 && errno == EINTR) {
        continue;
      } else {
        return -1;
      }
    }
    tot_written += num_written;
    buf += num_written;
  }
  return tot_written;
}

BootHalStatus setSide(unsigned int upTableID[]) {
  BootHalStatus ret = BOOTHAL_RET_ERR_FAIL;
  int binfo_fd = -1;
  int mmc_fd = -1;
  int sector_count = 0;
  char *buf = NULL;
  char binfo_file[FILEPATH_MAX] = {0};
  off_t mmc_offset;

  if (NULL == upTableID) {
    return BOOTHAL_RET_ERR_PARAM;
  }

  if (upTableID[BOOTHAL_SIDE_IDX_MAINSOFT] == BOOTHAL_SIDE_A) {
    snprintf(binfo_file, FILEPATH_MAX, "%s", BOOTINFO_FILEA);
  } else if (upTableID[BOOTHAL_SIDE_IDX_MAINSOFT] == BOOTHAL_SIDE_B) {
    snprintf(binfo_file, FILEPATH_MAX, "%s", BOOTINFO_FILEB);
  } else {
    ret = BOOTHAL_RET_ERR_PARAM;
    goto exit;
  }

  binfo_fd = open(binfo_file, O_RDONLY);
  if (binfo_fd < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "open %s:%s", binfo_file, strerror(errno));
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  mmc_fd = open(BOOT_PARTATION, O_WRONLY);
  if (mmc_fd < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "open %s:%s", BOOT_PARTATION, strerror(errno));
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  /*
   * enable mmcblk0boot1 for write
   */
  if (EnableMmcBlk(MMCBLK0BOOT1_FORCE_RO) < 0) {
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  /*
   * Get sector count of mmcblk0boot1 block from sys-file, every sector size is 512byte.
   * The mmcblk0boot1 block default size is 16M.
   */
  if ((sector_count = GetMmcBlkCount(MMCBLK0BOOT1_SIZE_PATH)) < 0) {
    mmc_offset = MMCBLK0BOOT1_SIZE_DEF - BOOTINFO_SIZE;
  } else {
    mmc_offset = sector_count * SECTOR_SIZE - BOOTINFO_SIZE;
  }

  /*
   * The environment should be write to the latest 128K of mmcblk0boot1.
   */
  if (lseek(mmc_fd, mmc_offset, SEEK_SET) < 0) {
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  buf = reinterpret_cast<char *>(mmap(NULL, BOOT_COPYBUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  if (buf == MAP_FAILED) {
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  while (1) {
    ssize_t rd, wr;

    rd = BootReadN(binfo_fd, buf, BOOT_COPYBUF_SIZE);
    if (!rd) {
      // EOF
      break;
    }
    if (rd < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "read:%s", strerror(errno));
      ret = BOOTHAL_RET_ERR_FAIL;
      goto exit;
    }

    wr = BootWriteN(mmc_fd, buf, rd);
    if (wr < rd) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "write(%s):%s", BOOT_PARTATION, strerror(errno));
      ret = BOOTHAL_RET_ERR_WRITE;
      goto exit;
    }
  }

  if (fsync(mmc_fd) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "fsync:%s", strerror(errno));
  }
  ret = BOOTHAL_RET_SUCCESS;

exit:
  if (binfo_fd >= 0) {
    if (close(binfo_fd) < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "close:%s", strerror(errno));
    }
  }

  if (mmc_fd >= 0) {
    if (close(mmc_fd) < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "close:%s", strerror(errno));
    }
  }

  if (buf) {
    munmap(buf, BOOT_COPYBUF_SIZE);
  }

  return ret;
}

BootHalStatus getSide(unsigned int upTableID[]) {
  BootHalStatus ret = BOOTHAL_RET_SUCCESS;
  int fd;
  int rd_size;
  char buf[CMDLINE_MAX_SIZE] = {0};
  char *rootfs = NULL;

  if (NULL == upTableID) {
    return BOOTHAL_RET_ERR_PARAM;
  }

  if ((fd = open(BOOT_CMDLINE, O_RDONLY)) < 0) {
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  if ((rd_size = BootReadN(fd, buf, CMDLINE_MAX_SIZE)) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "read %s error:%s", BOOT_CMDLINE, strerror(errno));
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  rootfs = strstr(buf, BOOTARGS_LST_ROOT);
  if (!rootfs) {
    ret = BOOTHAL_RET_ERR_FAIL;
    goto exit;
  }

  rootfs += strlen(BOOTARGS_LST_ROOT);
  if (strncmp(rootfs, BOOT_ROOTFS_A, strlen(BOOT_ROOTFS_A)) == 0 &&
      (rootfs[strlen(BOOT_ROOTFS_A)] == ' ' || rootfs[strlen(BOOT_ROOTFS_A)] == '\0')) {
    upTableID[BOOTHAL_SIDE_IDX_MAINSOFT] = BOOTHAL_SIDE_A;
  } else if (strncmp(rootfs, BOOT_ROOTFS_B, strlen(BOOT_ROOTFS_B)) == 0 &&
      (rootfs[strlen(BOOT_ROOTFS_B)] == ' ' || rootfs[strlen(BOOT_ROOTFS_B)] == '\0')) {
    upTableID[BOOTHAL_SIDE_IDX_MAINSOFT] = BOOTHAL_SIDE_B;
  } else {
    ret = BOOTHAL_RET_ERR_FAIL;
  }

exit:
  if (fd >= 0) {
    if (close(fd) < 0) {
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "close:%s", strerror(errno));
    }
  }

  return ret;
}

BootHalStatus setBootColdStart(void) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
  return BOOTHAL_RET_SUCCESS;
}

BootHalStatus getBootMode(uint32_t *mode) {
  if (NULL == mode) {
    return BOOTHAL_RET_ERR_PARAM;
  }

  *mode = COLD_START;
  return BOOTHAL_RET_SUCCESS;
}
