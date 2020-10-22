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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "ss_resm_resourcemanagerlog.h"

#define DROP_CACHES "/proc/sys/vm/drop_caches"

// FRAMEWORKUNIFIEDLOG
FRAMEWORKUNIFIEDLOGPARAM g_FrameworkunifiedLogParams = {
  FRAMEWORKUNIFIEDLOGOPTIONS,
  {
    ZONE_TEXT_10, ZONE_TEXT_11, ZONE_TEXT_12,
    ZONE_TEXT_13, ZONE_TEXT_14, ZONE_TEXT_15,
    ZONE_TEXT_16, ZONE_TEXT_17, ZONE_TEXT_18,
    ZONE_TEXT_19, ZONE_TEXT_20, ZONE_TEXT_21,
    ZONE_TEXT_22, ZONE_TEXT_23, ZONE_TEXT_24,
    ZONE_TEXT_25, ZONE_TEXT_26, ZONE_TEXT_27,
    ZONE_TEXT_28, ZONE_TEXT_29, ZONE_TEXT_30,
    ZONE_TEXT_31
  },
  FRAMEWORKUNIFIEDLOGZONES
};

// echo 1 > /proc/sys/vm/drop_caches
#define DROP_CACHES_INTVAL_TIMESEC (1 * 10)  // 10 sec

#define MEMINFO_FILE "/proc/meminfo"

/* memFree threshold */
#define MEMFREE_THRETHOLD (100*1024)
/* LowFree - CmaFree threshold */
#define LOWFREE_THRETHOLD (50*1024)

static uint32_t mainFree_kib;
static uint32_t lowFree_kib;
static uint32_t cmaFree_kib;
// meminfo table
typedef struct {
  const char* name;
  uint32_t* value;
} meminfo_tbl;

static int32_t get_meminfo(void);
static int32_t comp_meminfo_tbl(const void* data1, const void* data2);
static bool judge_cachedrop(void);

static void
exec_drop_caches(void) {
  int fd;

  fd = open(DROP_CACHES, O_RDWR);
  if (fd >= 0) {
    write(fd, (const void *)"1", 1);
    close(fd);
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "%s open error (fd:%d, errno:%d)", DROP_CACHES, fd, errno);
  }
}

int
main(int argc, char* argv[]) {
  const struct timespec drop_caches_intval = {DROP_CACHES_INTVAL_TIMESEC, 0};

  FRAMEWORKUNIFIED_SET_ZONES();

  while (1) {
    /* Get meminfo */
    get_meminfo();
    if (judge_cachedrop()) {
      exec_drop_caches();
    }
    nanosleep(&drop_caches_intval, NULL);
  }

  return 0;
}

/* Compare memory information */
static int32_t
comp_meminfo_tbl(const void* data1, const void* data2) {
  return strcmp(((const meminfo_tbl*)data1)->name,
                ((const meminfo_tbl*)data2)->name);
}

/* Get memory information */
static int32_t
get_meminfo(void) {
  int32_t meminfo_fd = -1;
  char srch_name[16];
  char buf[2048];
  meminfo_tbl target = {srch_name, NULL};
  meminfo_tbl* found;
  char* head;
  char* tail;
  int32_t read_byte;
  /* Strings must be in ascending order when adding entries to this table (for bsearch) */
  static const meminfo_tbl mem_table[] = {
    {"CmaFree",  &cmaFree_kib},
    {"LowFree",  &lowFree_kib},
    {"MemFree",  &mainFree_kib},
  };
  const int32_t mem_table_count = sizeof(mem_table)/sizeof(meminfo_tbl);

  if (meminfo_fd == -1) {
    meminfo_fd = open(MEMINFO_FILE, O_RDONLY);
    if (meminfo_fd == -1) {
      fflush(NULL);
      return -1;
    }
  }

  lseek(meminfo_fd, 0L, SEEK_SET);
  read_byte = read (meminfo_fd, buf, sizeof(buf) - 1);
  if (read_byte < 0) {
    fflush(NULL);
    close(meminfo_fd);
    return -1;
  }
  buf[read_byte] = '\0';

  head = buf;
  while (1) {
    tail = strchr(head, ':');
    if (!tail) {
      break;
    }
    *tail = '\0';
    if (strlen (head) >= sizeof(srch_name)) {
      head = tail + 1;
    } else {
      strcpy(srch_name, head);  // NOLINT
      found = reinterpret_cast<meminfo_tbl *>(bsearch(&target, mem_table, mem_table_count,
                                      sizeof(meminfo_tbl), comp_meminfo_tbl));
      head = tail + 1;
      if (found) {
        *(found->value) = strtoul(head, &tail, 10);
      }
    }
    tail = strchr(head, '\n');
    if (!tail)
      break;
    head = tail + 1;
  }
  close(meminfo_fd);

  return 0;
}

static bool
judge_cachedrop(void) {
  if (mainFree_kib < MEMFREE_THRETHOLD) {
    return true;
  }

  if ((lowFree_kib - cmaFree_kib) < LOWFREE_THRETHOLD) {
    return true;
  }

  return false;
}
