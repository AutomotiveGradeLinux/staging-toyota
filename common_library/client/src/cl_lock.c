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

// #include <stdio.h>
// #include <stdlib.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include <native_service/cl_lock.h>
#include <native_service/cl_lockid.h>
#include "cl_lock_internal.h"
#include "cl_error.h"

static int      shm_id = -1;

/*
 * Lock file is consists of slots(4KB)
 * The slot layout is:
 *   0     ~   4Byte : field of PID
 *   4Byte ~  28Byte : field of pthread_mutex_t
 */


static int cl_LockfileInit(void *base) {
  int   i;
  /*
    int   j;
  */
  void *addr;
  pthread_mutexattr_t   attr;

  if (pthread_mutexattr_init(&attr) != 0) {  // LCOV_EXCL_BR_LINE 5:fail safe for libc pthread_mutexattr_init
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;  // LCOV_EXCL_LINE 5:fail safe for libc pthread_mutexattr_init
  }
  if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {  // LCOV_EXCL_BR_LINE 5:fail safe for libc pthread_mutexattr_setpshared
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;  // LCOV_EXCL_LINE 5:fail safe for libc pthread_mutexattr_setpshared
  }
//  if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP) != 0) {
//    return -1;
//  }
  if (pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST) != 0) {
    return -1;
  }

  for (i = 0; i < LID_NUM; i++) {
    /*
        addr = SLOT_SIZE * 1 + base;
        for (j = 0; j < 1024; j++) {
          *(int*)(addr + j * sizeof(int)) = j;
        }
    */
    addr = SLOT_SIZE * i + (char *)base + sizeof(int);
    pthread_mutex_init((pthread_mutex_t *)addr, &attr);

  }
  if (pthread_mutexattr_destroy(&attr) != 0) { // LCOV_EXCL_BR_LINE 5:fail safe for libc pthread_mutexattr_destroy
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1; // LCOV_EXCL_LINE 5:fail safe for libc pthread_mutexattr_destroy
  }
  return 0;
}

/*
 * Initialize in the system
 *   This function will generate the Lock file, and initialize pthread_mutex_t for all slots
 */
int CL_LockSystemInit(void) {
  int           fd = -1;
  int           ret = 0;
  void         *base = MAP_FAILED;

  fd = shm_open(LOCKFILE_NAME, O_CREAT | O_EXCL | O_RDWR, (S_IRWXG | S_IRWXO | S_IRWXU));
  if (fd < 0) {
    ret = -1;
    goto exit;
  }
  if (ftruncate(fd, LOCKFILE_SIZE) != 0) { // LCOV_EXCL_BR_LINE 5:fail safe for libc ftruncate
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    ret = -1;  // LCOV_EXCL_LINE 5:fail safe for libc ftruncate
    goto exit;  // LCOV_EXCL_LINE 5:fail safe for libc ftruncate
  }

  base = mmap(NULL, LOCKFILE_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
  if (base == MAP_FAILED) {  // LCOV_EXCL_BR_LINE 5:fail safe for libc mmap
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    ret = -1;  // LCOV_EXCL_LINE 5:fail safe for libc mmap
    goto exit;  // LCOV_EXCL_LINE 5:fail safe for libc mmap
  }

  if (cl_LockfileInit(base) < 0) { // LCOV_EXCL_BR_LINE 11:out branch
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    ret = -1;  // LCOV_EXCL_LINE 5:fail safe for libc
    goto exit;  // LCOV_EXCL_LINE 5:fail safe for libc
  }

exit:
  if (fd >= 0) {
    close(fd);
  }
  if (base != MAP_FAILED) {
    munmap(base, LOCKFILE_SIZE);
  }
  return ret;
}

void CL_LockSystemFin_debug(void) {
  if (shm_id >= 0) {
    close(shm_id);
  }
  shm_unlink(LOCKFILE_NAME);
  shm_id = -1;
}

/*
 * Initialize in the process
 *   Open the Lock file
 */
int CL_LockProcessInit(void) {
  if (shm_id < 0) {
    shm_id = shm_open(LOCKFILE_NAME, O_RDWR, (S_IRWXG | S_IRWXO | S_IRWXU));
  }
  if (shm_id < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function shm_open
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    return -1;  // LCOV_EXCL_LINE 5: fail safe for glibc function shm_open
  }
  return 0;
}

void *CL_LockMap(int lid) {
  if ((lid < 0) || (lid >= LID_NUM)) {
    errno = EINVAL;
    return MAP_FAILED;
  }
  return mmap(NULL, SLOT_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, shm_id, (off_t)(lid * SLOT_SIZE));
}

int CL_LockUnmap(void *addr) {
  return munmap(addr, SLOT_SIZE);
}

int CL_LockGet(void *addr) {
  int ret;

  if ((addr == NULL) || (addr == MAP_FAILED)) {
    ret = EINVAL;
  } else {
    CL_DBG_PRINT("@@@@@ %s Start: pid = %d\n", __func__, *(int *)addr);
    // LCOV_EXCL_BR_START 5:fail safe for libc pthread_mutex_lock
    if ((ret = pthread_mutex_lock((pthread_mutex_t*)((char *)addr + sizeof(int)))) == 0) {
      // LCOV_EXCL_BR_STOP
      *(int *)addr = (int)getpid();
    } else if (ret == EOWNERDEAD) {
      if ((ret = pthread_mutex_consistent((pthread_mutex_t *)((char *)addr + sizeof(int)))) == 0) {
        *(int *)addr = (int)getpid();
      }
    }
    CL_DBG_PRINT("@@@@@ %s End: pid = %d\n", __func__, *(int *)addr);
  }
  return ret;
}

int CL_LockNowait(void *addr) {
  int ret;

  if ((addr == NULL) || (addr == MAP_FAILED)) {
    ret = EINVAL;
  } else {
    CL_DBG_PRINT("@@@@@ %s Start: pid = %d\n", __func__, *(int *)addr);
    if ((ret = pthread_mutex_trylock((pthread_mutex_t*)((char *)addr + sizeof(int)))) == 0) {
      *(int *)addr = (int)getpid();
    } else if (ret == EOWNERDEAD) {
      if ((ret = pthread_mutex_consistent((pthread_mutex_t *)((char *)addr + sizeof(int)))) == 0) {
        *(int *)addr = (int)getpid();
      }
    }
    CL_DBG_PRINT("@@@@@ %s End: pid = %d\n", __func__, *(int *)addr);
  }
  return ret;
}


int CL_LockRelease(void *addr) {
  int ret;
  if ((addr == NULL) || (addr == MAP_FAILED)) {
    ret = EINVAL;
  } else {
    CL_DBG_PRINT("@@@@@ %s : pid = %d\n", __func__, *(int *)addr);
    *(int *)addr = (int)0;
    ret = pthread_mutex_unlock((pthread_mutex_t*)((char *)addr + sizeof(int)));
  }
  return ret;
}



