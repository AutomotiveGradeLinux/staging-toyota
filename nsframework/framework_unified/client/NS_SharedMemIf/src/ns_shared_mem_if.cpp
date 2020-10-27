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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSSharedMemory
/// \brief SharedMemIf implementation
///
/// Implements interface functions to SharedMem service
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/ns_shared_mem_if.h>         ///< Shared mem source interface file
#include <native_service/ns_logger_if.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define NS_SHM_HEADER_SIZE (48)

/////////////////////////////////////////////////////////////////////////////
/// Utility functions and classes

static TMemID getclock(void) {
  struct timespec tp;
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &tp) < 0) {   // LCOV_EXCL_BR_LINE 5: clock_gettime's error case.
    return 0;
  }
  return (TMemID)(((uint64_t)tp.tv_sec * 1000 * 1000 * 1000 + tp.tv_nsec) >> 8);
}

static size_t uitoa(unsigned int value, char *buf) {
  static const char c[] = "0123456789abcdef";
  int i;

  if (buf == NULL) {
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "buf is NULL");
      return 0;
  }
  for (i = 0; i < 8; i++) {
    buf[i] = c[(value >> ((7 - i) * 4)) & 0xf];
  }
  buf[i] = '\0';

  return i;
}

static inline void create_shmname(char *buf, TMemID id) {
  if (buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "buf is NULL");
    return;
  }
  strcpy(buf, "nsshm_");  // NOLINT  (readability/nolint)
  buf += sizeof("nsshm_") - 1;
  uitoa(id, buf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Public interface function definitions

TMemID SetDataToShared(const void *data, UI_32 dataBytes, const char *from, const char *to) {
  TMemID id;
  char shmname[16];
  int fd;
  void *addr = MAP_FAILED;

  if (data == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "data is NULL");
    return BAD_MEM_ID;
  }
  if (from == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "from is NULL");
    return BAD_MEM_ID;
  }
  if (to == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "to is NULL");
    return BAD_MEM_ID;
  }

  if (dataBytes == 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "DataSize is invalied(%d)", dataBytes);
    return BAD_MEM_ID;
  }

  while (1) {
    id = getclock();
    create_shmname(shmname, id);

    if ((fd = shm_open(shmname, O_CREAT | O_EXCL | O_RDWR, 0666)) < 0) {
      if (errno == EEXIST) {
        continue;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "shm_open: %s", strerror(errno));
      return BAD_MEM_ID;
    }
    break;
  }

  if (ftruncate(fd, dataBytes + NS_SHM_HEADER_SIZE) < 0) {  // LCOV_EXCL_BR_LINE 5: ftruncate's error case.
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "ftruncate: %s", strerror(errno));
    id = BAD_MEM_ID;
    goto exit;
  }

  if ((addr = mmap(NULL, dataBytes + NS_SHM_HEADER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "mmap: %s", strerror(errno));
    id = BAD_MEM_ID;
    goto exit;
  }

  strcpy(reinterpret_cast<char *>(addr), from);  // NOLINT  (readability/nolint)
  strcpy(reinterpret_cast<char *>(addr) + (NS_SHM_HEADER_SIZE / 2), to);  // NOLINT  (readability/nolint)
  /**
   * @todo
   * Specifying a large number (4097 or higher) for the Session Data Size for transmission results in a segmentation fault.
   */
  memcpy(reinterpret_cast<char *>(addr) + NS_SHM_HEADER_SIZE, data, dataBytes);

exit:
  close(fd);
  if (addr != MAP_FAILED) {
    munmap(addr, dataBytes + NS_SHM_HEADER_SIZE);
  }
  if (id == BAD_MEM_ID) {
    shm_unlink(shmname);
  }
  return id;
}

EFrameworkunifiedStatus GetDataFromShared(TMemID id, void *data, UI_32 dataMaxBytes) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  char shmname[16];
  int fd;
  void *addr = MAP_FAILED;
  struct stat sb;

  if (id == BAD_MEM_ID) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "ShMemID is invalied(%u)", id);
    return eFrameworkunifiedStatusInvldParam;
  }
  if (data == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "data is NULL");
    return eFrameworkunifiedStatusInvldParam;
  }
  if (dataMaxBytes == 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "DataSize is invalied(%d)", dataMaxBytes);
    return eFrameworkunifiedStatusInvldParam;
  }

  create_shmname(shmname, id);

  if ((fd = shm_open(shmname, O_RDONLY, 0)) < 0) {
    if (errno == ENOENT) {
      eStatus = eFrameworkunifiedStatusInvldID;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "shm_open: %s", strerror(errno));
    return eStatus;
  }

  if (fstat(fd, &sb) < 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "fstat: %s", strerror(errno));
    goto exit;
  }
  if (sb.st_size - NS_SHM_HEADER_SIZE > static_cast<SI_64>(dataMaxBytes)) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "DataSize is invalied(%d-%d)", dataMaxBytes, (int)sb.st_size);
    eStatus = eFrameworkunifiedStatusInvldParam;
    goto exit;
  }

  if ((addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5: mmap's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "mmap: %s", strerror(errno));  // LCOV_EXCL_LINE 5: mmap's error case.
    goto exit;
  }

  memcpy(data, reinterpret_cast<char *>(addr) + NS_SHM_HEADER_SIZE, sb.st_size - NS_SHM_HEADER_SIZE);
  eStatus = eFrameworkunifiedStatusOK;

exit:
  close(fd);
  if (addr != MAP_FAILED) {
    munmap(addr, sb.st_size);
  }
  return eStatus;
}

UI_32 GetLengthOfDataFromShared(TMemID id) {
  UI_32 size = 0;
  char shmname[16];
  int fd;
  struct stat sb;

  if (id == BAD_MEM_ID) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "ShMemID is invalied(%u)", id);  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    return 0;
  }

  create_shmname(shmname, id);

  if ((fd = shm_open(shmname, O_RDWR, 0)) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "shm_open: %s", strerror(errno));
    return 0;
  }

  if (fstat(fd, &sb) < 0) {  // LCOV_EXCL_BR_LINE 5: fstat's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "fstat: %s", strerror(errno));  // LCOV_EXCL_LINE 5: mmap's error case.
    goto exit;
  }

  size = static_cast<UI_32>(sb.st_size - NS_SHM_HEADER_SIZE);

exit:
  close(fd);
  return size;
}

EFrameworkunifiedStatus DiscardDataFromShared(TMemID id) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;
  char shmname[16];

  if (id == BAD_MEM_ID) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "ShMemID is invalied(%u)", id);
    return eFrameworkunifiedStatusInvldParam;
  }

  create_shmname(shmname, id);

  if (shm_unlink(shmname) < 0) {
    if (errno == ENOENT) {
      eStatus = eFrameworkunifiedStatusInvldID;
    } else {
      eStatus = eFrameworkunifiedStatusFail;
    }
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "shm_unlink: %s", strerror(errno));
  }

  return eStatus;
}
