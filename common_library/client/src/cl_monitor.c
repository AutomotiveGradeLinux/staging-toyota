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

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>

#include "cl_error.h"
#include <native_service/cl_region.h>
#include <native_service/cl_monitor.h>

#define CL_MONITOR_SHM_NAME "/cl_monitor"

#define CL_MONITOR_ID_REAL_GENERIC_START (0)
#define CL_MONITOR_ID_REAL_GENERIC_END   (1023)
#define CL_MONITOR_ID_REAL_RPC_START     (49952)
#define CL_MONITOR_ID_REAL_RPC_END       (59999)

#define CL_MONITOR_ID_FLAT_GENERIC_START (0)
#define CL_MONITOR_ID_FLAT_GENERIC_END \
  (CL_MONITOR_ID_FLAT_GENERIC_START + (CL_MONITOR_ID_REAL_GENERIC_END - CL_MONITOR_ID_REAL_GENERIC_START))
#define CL_MONITOR_ID_FLAT_RPC_START (CL_MONITOR_ID_FLAT_GENERIC_END + 1)
#define CL_MONITOR_ID_FLAT_RPC_END \
  (CL_MONITOR_ID_FLAT_RPC_START + (CL_MONITOR_ID_REAL_RPC_END - CL_MONITOR_ID_REAL_RPC_START))
#define CL_MONITOR_ENTRY_NUM (CL_MONITOR_ID_FLAT_RPC_END + 1)

#define CL_MONITOR_BITMAP_LENGTH ((CL_MONITOR_ENTRY_NUM + 63) / 64)  // aligned

#define CL_ALIGN(x, a) (((x) + (a - 1)) / (a) * (a))

struct cl_monitor_header {
  char signature[4];
  uint64_t bitmap[CL_MONITOR_BITMAP_LENGTH];
  sem_t sem;
};

#define CL_MONITOR_OBJECT_SIZE \
  CL_ALIGN((sizeof(struct cl_monitor_header) + (sizeof(CL_MonitorEntry_t) * CL_MONITOR_ENTRY_NUM)), 4096)

#define CL_MONITOR_SET_BIT(a, i) (a[i / 64] |= (1ULL << (i % 64)))
#define CL_MONITOR_CLEAR_BIT(a, i) (a[i / 64] &= ~(1ULL << (i % 64)))

static struct cl_monitor_header *cl_monitor_obj;
static CL_MonitorEntry_t *cl_monitor_entry_head;

int CL_MonitorInit(CL_MonitorInit_t init_type) {
  int fd;
  int oflag;
  mode_t mode;

  if (init_type != CL_MONITOR_INIT_SYSTEM && init_type != CL_MONITOR_INIT_USER) {
    errno = EINVAL;
    return -1;
  }

  if (cl_monitor_obj != NULL) {
    return 0;
  }

  if (init_type == CL_MONITOR_INIT_SYSTEM) {
    oflag = O_RDWR | O_CREAT | O_EXCL;
    mode = 0666;
  } else {
    oflag = O_RDWR;
    mode = 0;
  }

  if ((fd = shm_open(CL_MONITOR_SHM_NAME, oflag, mode)) == -1) { // LCOV_EXCL_BR_LINE 5: fail safe for glibc function shm_open
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function shm_open
  }

  if (init_type == CL_MONITOR_INIT_SYSTEM) {
    if (ftruncate(fd, CL_MONITOR_OBJECT_SIZE) == -1) { // LCOV_EXCL_BR_LINE 5:  fail safe for glibc function ftruncate
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      return -1; // LCOV_EXCL_LINE 5:  fail safe for glibc function ftruncate
    }
  } else {
    struct stat st_buf;
    if (fstat(fd, &st_buf) == -1) { // LCOV_EXCL_BR_LINE 5:  fail safe for glibc function fstat
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      return -1; // LCOV_EXCL_LINE 5:  fail safe for glibc function fstat
    }

    if (st_buf.st_size != CL_MONITOR_OBJECT_SIZE) {
      errno = EAGAIN;
      return -1;
    }
  }

  // LCOV_EXCL_BR_START 5:  fail safe for glibc function mmap
  if ((cl_monitor_obj = mmap(NULL, CL_MONITOR_OBJECT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    // LCOV_EXCL_BR_STOP
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1; // LCOV_EXCL_LINE 5:  fail safe for glibc function mmap
  }

  if (init_type == CL_MONITOR_INIT_SYSTEM) { // LCOV_EXCL_BR_LINE 11:  out branch
    memcpy(cl_monitor_obj->signature, "CLAM", 4);
    memset(cl_monitor_obj->bitmap, 0, sizeof(cl_monitor_obj->bitmap));
    sem_init(&cl_monitor_obj->sem, 1, 1);
  }
  cl_monitor_entry_head = (CL_MonitorEntry_t *)(((char *)cl_monitor_obj) + sizeof(struct cl_monitor_header));

  return 0;
}

static inline int cl_monitor_check_type_id(CL_MonitorType_t type, uint32_t id, int *offset) {
  if (type == CL_MONITOR_TYPE_GENERIC) {
    if (id > CL_MONITOR_ID_REAL_GENERIC_END) {
      errno = ENOENT;
      return -1;
    }
    *offset = (int)((id - CL_MONITOR_ID_REAL_GENERIC_START) + CL_MONITOR_ID_FLAT_GENERIC_START);
  } else if (type == CL_MONITOR_TYPE_RPC) {
    if (id < CL_MONITOR_ID_REAL_RPC_START || id > CL_MONITOR_ID_REAL_RPC_END) {
      errno = ENOENT;
      return -1;
    }
    *offset = (int)((id - CL_MONITOR_ID_REAL_RPC_START) + CL_MONITOR_ID_FLAT_RPC_START);
  } else {
    errno = ENOENT;
    return -1;
  }

  return 0;
}

static inline int cl_monitor_sem_wait(sem_t *sem) {
  int ret;

retry:
  ret = sem_wait(sem);
  if (ret == -1) { // LCOV_EXCL_BR_LINE 5: fail safe for libc   sem_wait
    // LCOV_EXCL_START 5: fail safe for libc   sem_wait

    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    if (errno == EINTR) {
      goto retry;
    }
    CL_PERROR("sem_wait");
  }
  // LCOV_EXCL_STOP

  return ret;
}

static inline int cl_monitor_sem_post(sem_t *sem) {
  int ret;

  if ((ret = sem_post(sem)) == -1) { // LCOV_EXCL_BR_LINE 5: fail safe for libc sem_post
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    CL_PERROR("sem_post");  // LCOV_EXCL_LINE 5: fail safe for libc sem_post
  }

  return ret;
}

int CL_MonitorSetEntry(CL_MonitorType_t type, uint32_t id, CL_MonitorState_t state, uint32_t timeout,
                       uint32_t user_data) {
  int offset;
  CL_MonitorEntry_t *e;
  struct timespec ts;

  if (cl_monitor_obj == NULL) {
    errno = ENOENT;
    return -1;
  }

  if (cl_monitor_check_type_id(type, id, &offset) == -1) {
    return -1;
  }

  if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) { // LCOV_EXCL_BR_LINE 5: fail safe for libc clock_gettime
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1; // LCOV_EXCL_LINE 5: fail safe for libc clock_gettime
  }

  e = cl_monitor_entry_head + offset;
  cl_monitor_sem_wait(&cl_monitor_obj->sem);

  if (state == CL_MONITOR_STATE_SLEEP) {
    memset(e, 0, sizeof(CL_MonitorEntry_t));
    CL_MONITOR_CLEAR_BIT(cl_monitor_obj->bitmap, offset);
  } else {
    e->pid = (uint16_t)getpid();
    e->type = type;
    e->state = state;

//    e->timeout = (uint32_t)ts.tv_sec + timeout;
    e->timeout = ts.tv_sec + timeout;
    e->id = id;
    e->user_data = user_data;
    CL_MONITOR_SET_BIT(cl_monitor_obj->bitmap, offset);
  }

  cl_monitor_sem_post(&cl_monitor_obj->sem);

  return 0;
}

int CL_MonitorGetEntry(CL_MonitorType_t type, uint32_t id, CL_MonitorEntry_t *entry) {
  int offset;
  CL_MonitorEntry_t *e;

  if (cl_monitor_obj == NULL) {
    errno = ENOENT;
    return -1;
  }

  if (entry == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (cl_monitor_check_type_id(type, id, &offset) == -1) {
    return -1;
  }

  e = cl_monitor_entry_head + offset;
  cl_monitor_sem_wait(&cl_monitor_obj->sem);

  memcpy(entry, e, sizeof(CL_MonitorEntry_t));

  cl_monitor_sem_post(&cl_monitor_obj->sem);

  return 0;
}

int CL_MonitorSearchInit(CL_MonitorSearch_t *serch) {
  if (serch == NULL) {
    errno = EINVAL;
    return -1;
  }

  serch->entry_list = NULL;
  serch->entry_num = 0;
  return 0;
}

int CL_MonitorSearchDestroy(CL_MonitorSearch_t *serch) {
  if (serch == NULL) {
    errno = EINVAL;
    return -1;
  }

  free(serch->entry_list);
  serch->entry_num = 0;
  return 0;
}

static inline int cl_monitor_popcnt64(uint64_t x) {
  uint64_t n;

  n = (x >> 1) & 0x7777777777777777ULL;
  x = x - n;
  n = (n >> 1) & 0x7777777777777777ULL;
  x = x - n;
  n = (n >> 1) & 0x7777777777777777ULL;
  x = x - n;
  x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
  x = x * 0x0101010101010101ULL;
  return (int)(x >> 56);
}

typedef struct cl_monitor_offset_s cl_monitor_offset_t;
struct cl_monitor_offset_s {
  cl_monitor_offset_t *next;
  int offset;
};

int CL_MonitorSearchTimeout(CL_MonitorSearch_t *search) {
  int i;
  int offset;
  CL_MonitorEntry_t *e;
  struct timespec ts;
  int timeout_entry_num = 0;
  cl_region_t *r;
  cl_monitor_offset_t *o, *head = NULL, *tail = NULL;

  if (cl_monitor_obj == NULL) {
    errno = ENOENT;
    return -1;
  }

  if (search == NULL) {
    errno = EINVAL;
    return -1;
  }

  if ((r = CL_RegionCreate(CL_REGION_DEFAULT_SIZE)) == NULL) {
    errno = ENOMEM;
    return -1;
  }

  free(search->entry_list);
  search->entry_list = NULL;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {// LCOV_EXCL_BR_LINE 5: fail safe for libc clock_gettime
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;// LCOV_EXCL_LINE 5: fail safe for libc clock_gettime
  }

  cl_monitor_sem_wait(&cl_monitor_obj->sem);

  for (i = 0; i < CL_MONITOR_BITMAP_LENGTH; i++) {
    if (cl_monitor_obj->bitmap[i] != 0) {
      uint64_t bits, mrb1;  // most right bit 1

      bits = cl_monitor_obj->bitmap[i];
      while (bits) {
        mrb1 = bits & (-bits);
        offset = i * 64 + cl_monitor_popcnt64(mrb1 - 1);
        e = cl_monitor_entry_head + offset;

        if (e->timeout <= ts.tv_sec) {
          timeout_entry_num++;
          if ((o = CL_RegionAlloc(r, cl_monitor_offset_t, 1)) == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for mmap
            // LCOV_EXCL_START 5: fail safe for libc mmap
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            errno = ENOMEM;
            timeout_entry_num = -1;
            goto exit;
            // LCOV_EXCL_STOP
          }

          o->offset = offset;
          o->next = NULL;

          if (head == NULL) {
            head = tail = o;
          } else {
            tail->next = o;
            tail = o;
          }
        }
        bits &= ~mrb1;
      }
    }
  }

  if (timeout_entry_num) {
    CL_MonitorEntry_t *src, *dst;
    dst = malloc(sizeof(CL_MonitorEntry_t) * (size_t)timeout_entry_num);
    if (dst == NULL) {  // LCOV_EXCL_LINE 5: fail safe for libc malloc
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      errno = ENOMEM;  // LCOV_EXCL_LINE 5: fail safe for libc malloc
      timeout_entry_num = -1;  // LCOV_EXCL_LINE 5: fail safe for libc malloc
      goto exit;  // LCOV_EXCL_LINE 5: fail safe for libc malloc
    }

    search->entry_list = dst;
    o = head;
    for (i = 0; i < timeout_entry_num; i++) {
      src = cl_monitor_entry_head + o->offset;
      memcpy(dst, src, sizeof(CL_MonitorEntry_t));

      o = o->next;
      dst++;
    }
  }

exit:
  cl_monitor_sem_post(&cl_monitor_obj->sem);

  CL_RegionDestroy(r);
  search->entry_num = (timeout_entry_num == -1) ? 0 : timeout_entry_num;  // LCOV_EXCL_BR_LINE 11:  out branch
  return timeout_entry_num;
}

int cl_monitor_cleanup(int pid) {
  int ret = 0;
  int i;
  int offset;
  CL_MonitorEntry_t *e;

  if (cl_monitor_obj == NULL) {
    errno = ENOENT;
    return -1;
  }

  if (pid <= 0) { // LCOV_EXCL_LINE 5: fail safe for glibc function waitid
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    errno = EINVAL;// LCOV_EXCL_LINE 5: fail safe for glibc function waitid
    return -1;// LCOV_EXCL_LINE 5: fail safe for glibc function waitid
  }

  cl_monitor_sem_wait(&cl_monitor_obj->sem);

  for (i = 0; i < CL_MONITOR_BITMAP_LENGTH; i++) {
    if (cl_monitor_obj->bitmap[i] != 0) {
      uint64_t bits, mrb1;  // most right bit 1

      bits = cl_monitor_obj->bitmap[i];
      while (bits) {
        mrb1 = bits & (-bits);
        offset = i * 64 + cl_monitor_popcnt64(mrb1 - 1);

        e = cl_monitor_entry_head + offset;
        if (e->pid == pid) {
          memset(e, 0, sizeof(CL_MonitorEntry_t));
          CL_MONITOR_CLEAR_BIT(cl_monitor_obj->bitmap, offset);
          ret++;
        }

        bits &= ~mrb1;
      }
    }
  }

  cl_monitor_sem_post(&cl_monitor_obj->sem);

  return ret;
}
