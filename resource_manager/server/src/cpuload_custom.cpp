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
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ss_resm_resourcemanagerlog.h"

#include "resm_cfg.h"

typedef struct _task_stat {
  struct _task_stat *next;
  char path[64];
  pid_t pid_nr_ns;
  char tcomm[64];
  char state;
  pid_t ppid;
  pid_t pgid;
  unsigned long utime;  // NOLINT
  unsigned long stime;  // NOLINT
  int priority;
  int nice;
  int task_cpu;
  unsigned int task_rt_priority;
  unsigned int task_policy;
} TASK_STAT;


typedef struct _task_rank
{
  pid_t pid_nr_ns;
  char state;
  unsigned long taskOccupancy;
  unsigned long utime;
  unsigned long stime;
  int priority;
  unsigned int task_policy;
  int task_cpu;
  char tcomm[64];
} TASK_RANK;


static TASK_STAT *g_task_stat = NULL;

static int32_t g_fifo_task_pid = -1;
static int32_t g_tss_task_pid = -1;
static unsigned long g_fifo_task_occ = -1;
static unsigned long g_tss_task_occ = -1;
static int32_t g_fifo_show_timer = 0;
static int32_t g_tss_show_timer = 0;
static unsigned int g_startTime = 0;
static char g_fifo_path[64];
static char g_tss_path[64];
static unsigned long g_fifo_utime;
static unsigned long g_fifo_stime;
static unsigned long g_tss_utime;
static unsigned long g_tss_stime;


static void cleanup_task_stat(void) {
  TASK_STAT *p, *q;

  p = g_task_stat;
  while (p) {
    q = p->next;
    free(p);
    p = q;
  }
  g_task_stat = NULL;
}

static int add_task_stat(TASK_STAT *tstat) {
  TASK_STAT *p;

  p = reinterpret_cast<TASK_STAT *>(malloc(sizeof(TASK_STAT)));
  if (p == NULL) {  // LCOV_EXCL_BR_LINE 5: malloc error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return -1;  // LCOV_EXCL_LINE 5: malloc error case
  }
  memcpy(p, tstat, sizeof(TASK_STAT));
  p->next = 0;

  if (g_task_stat) {
    p->next = g_task_stat;
  }
  g_task_stat = p;

  return 0;
}

// No error : > 0
static int get_task_stat(const char *path, TASK_STAT *tstat) {
  pid_t sid;
  int tty_nr, tty_pgrp;
  unsigned long task_flags;  // NOLINT
  unsigned long min_flt, cmin_flt, maj_flt, cmaj_flt;  // NOLINT
  signed long cutime, cstime;  // NOLINT
  int num_threads, zero1;
  unsigned long long start_time;  // NOLINT
  unsigned long vsize, mm_rss, rsslim, start_code, end_code, start_stack;  // NOLINT
  unsigned long esp, eip, task_pending_signal_sig, task_blocked_sig;  // NOLINT
  unsigned long sigign_sig, sigcatch_sig, wchan, zero2, zero3;  // NOLINT
  int exit_signal;
  unsigned long long delayacct_blkio_ticks;  // NOLINT
  unsigned long guest_time;  // NOLINT
  signed long cguest_time;  // NOLINT

  int fd;
  int ret = -1;
#define READ_MAX_SIZE 4096
  char buf[READ_MAX_SIZE];

  fd = open(path, O_RDONLY);
  if (fd != -1) {
    if (read(fd, buf, sizeof(buf)) != -1) {  // LCOV_EXCL_BR_LINE 5: read's error case
      ret =
          sscanf(
              buf,
              "%d %32s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %d %d %d %d %llu %lu %lu"
              " %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld",
              &(tstat->pid_nr_ns), tstat->tcomm, &(tstat->state),
              &(tstat->ppid), &(tstat->pgid), &sid, &tty_nr, &tty_pgrp,
              &task_flags, &min_flt, &cmin_flt, &maj_flt, &cmaj_flt,
              &(tstat->utime), &(tstat->stime), &cutime, &cstime,
              &(tstat->priority), &(tstat->nice), &num_threads, &zero1,
              &start_time, &vsize, &mm_rss, &rsslim, &start_code, &end_code,
              &start_stack, &esp, &eip, &task_pending_signal_sig,
              &task_blocked_sig, &sigign_sig, &sigcatch_sig, &wchan, &zero2,
              &zero3, &exit_signal, &(tstat->task_cpu),
              &(tstat->task_rt_priority), &(tstat->task_policy),
              &delayacct_blkio_ticks, &guest_time, &cguest_time);
    }
    close(fd);
  }
  return ret;
}

static void get_current_task_stat(void) {
  TASK_STAT tstat;
  DIR *dirp;
  struct dirent entry;
  struct dirent *result;
  char path[64];

  struct timespec tp;

  dirp = opendir("/proc");
  if (dirp == NULL) {  // LCOV_EXCL_BR_LINE 5: opendir error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;  // LCOV_EXCL_LINE 5: opendir error case
  }

  if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0) {
    g_startTime = ((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));  // Unit is msec
  } else {
    g_startTime = 0;
  }

  while ((readdir_r(dirp, &entry, &result) == 0) && (result != NULL)) {
    if ((entry.d_name[0] >= '1') && (entry.d_name[0] <= '9')) {
      DIR *dirp2;
      struct dirent entry2;
      struct dirent *result2;

      sprintf(path, "/proc/%s/task", entry.d_name);  // NOLINT
      dirp2 = opendir(path);
      if (dirp2 == NULL) {  // LCOV_EXCL_BR_LINE 5: opendir error case
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        continue;  // LCOV_EXCL_LINE 5: opendir error case
      }

      while ((readdir_r(dirp2, &entry2, &result2) == 0) && (result2 != NULL)) {
        if ((entry2.d_name[0] >= '1') && (entry2.d_name[0] <= '9')) {
          char path2[64];
          int ret;
          struct timespec req = { 0, 1000000 };  // 1msec

          sprintf(path2, "%s/%s/stat", path, entry2.d_name);  // NOLINT
          ret = get_task_stat(path2, &tstat);

//          if ((ret > 0) && (tstat.task_policy == SCHED_FIFO)) {
          if (ret > 0) {  // LCOV_EXCL_BR_LINE 200: ret must be bigger than 0

            strcpy(tstat.path, path2);  // NOLINT
            add_task_stat(&tstat);
          }
          nanosleep(&req, NULL);
        }
      }
      closedir(dirp2);
    }
  }
  closedir(dirp);
}

static void show_cpuload(void) {
  int ret;
  TASK_STAT tstat;
  TASK_STAT *p = g_task_stat;

  TASK_RANK ttaskRankData[TASK_STAT_RANK_NUM+1];
  TASK_RANK t_eva_taskRank;
  int setRankDatanum = 0;
  int checkCounter;

  unsigned long dtime;  // NOLINT
  unsigned long dtime_urank;
  unsigned long dtime_drank;
  struct timespec tp;
  unsigned int endTime = 0;

//  if (p) {
//    FRAMEWORKUNIFIEDLOG(
//        ZONE_INFO,
//        __FUNCTION__,
//        "[CpuHighLoad]Please check User backtrace of following processes in kernel.log");
//    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
//           "[CpuHighLoad]  PID  ST PRI POLICY cpuX %%CPU (ut:st) comm");
//  }
  if (clock_gettime (CLOCK_MONOTONIC, &tp) == 0) {
    endTime = ((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));  // Unit is msec
  } else {
    endTime = (WTC_CPU_INTERVAL * 1000) + g_startTime;
  }
  if (endTime - g_startTime <= 0) {  // LCOV_EXCL_BR_LINE 200: (endTime - g_startTime) must be bigger than 0
    // LCOV_EXCL_START 200: (endTime - g_startTime) must be bigger than 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG (ZONE_WARN, __FUNCTION__, "[CpuHighLoad] Timer abnormality.");
    endTime = g_startTime + 1;
    // LCOV_EXCL_STOP
  }

  while (p) {
    ret = get_task_stat(p->path, &tstat);
    if (ret >= 0) {

#if 0
      dtime = (tstat.utime + tstat.stime) - (p->utime + p->stime);
      if (dtime > 0) {
        FRAMEWORKUNIFIEDLOG(
            ZONE_INFO,
            __FUNCTION__,
            "[CpuHighLoad]%5d   %c %3d %5s  cpu%d %4lu (%lu:%lu) %s",
            p->pid_nr_ns,
            p->state,
            p->priority,
            p->task_policy == SCHED_FIFO ? "FIFO" :
            p->task_policy == SCHED_RR ? "RR" : "OTHER",
            p->task_cpu, dtime, tstat.utime - p->utime, tstat.stime - p->stime,
            p->tcomm);
        if ((dtime >= 70)  // %%CPU 70% over only
        && (getpid() != p->pid_nr_ns)  // except current process
            && (strcmp(p->tcomm, "(NS_BackupMgr)"))) {  // except "BackupMgr"
//      print_backtrace_pid (p->pid_nr_ns); // backtrace in kernel.log
        }
      }
    }
    p = p->next;
  }
#endif
      dtime = (((tstat.utime + tstat.stime) - (p->utime + p->stime)) * 1000) / (endTime - g_startTime);
      if (((tstat.utime + tstat.stime) - (p->utime + p->stime)) > 0) {
        if (setRankDatanum <= TASK_STAT_RANK_NUM) {
          ttaskRankData[setRankDatanum].pid_nr_ns = p->pid_nr_ns;
          ttaskRankData[setRankDatanum].state = p->state;
          ttaskRankData[setRankDatanum].priority = p->priority;
          ttaskRankData[setRankDatanum].task_policy = p->task_policy;
          ttaskRankData[setRankDatanum].task_cpu = p->task_cpu;
          ttaskRankData[setRankDatanum].taskOccupancy = dtime;
          ttaskRankData[setRankDatanum].utime = tstat.utime - p->utime;
          ttaskRankData[setRankDatanum].stime = tstat.stime - p->stime;
          strcpy(ttaskRankData[setRankDatanum].tcomm, p->tcomm);
          setRankDatanum++;
        } else {
          ttaskRankData[TASK_STAT_RANK_NUM].pid_nr_ns = p->pid_nr_ns;
          ttaskRankData[TASK_STAT_RANK_NUM].state = p->state;
          ttaskRankData[TASK_STAT_RANK_NUM].priority = p->priority;
          ttaskRankData[TASK_STAT_RANK_NUM].task_policy = p->task_policy;
          ttaskRankData[TASK_STAT_RANK_NUM].task_cpu = p->task_cpu;
          ttaskRankData[TASK_STAT_RANK_NUM].taskOccupancy = dtime;
          ttaskRankData[TASK_STAT_RANK_NUM].utime = tstat.utime - p->utime;
          ttaskRankData[TASK_STAT_RANK_NUM].stime = tstat.stime - p->stime;
          strcpy(ttaskRankData[TASK_STAT_RANK_NUM].tcomm, p->tcomm);
        }
        for (checkCounter = (setRankDatanum - 1); checkCounter > 0; checkCounter--) {
          dtime_urank = ttaskRankData[checkCounter - 1].utime + ttaskRankData[checkCounter - 1].stime;
          dtime_drank = ttaskRankData[checkCounter].utime + ttaskRankData[checkCounter].stime;
          if (dtime_urank < dtime_drank) {
            t_eva_taskRank.pid_nr_ns = ttaskRankData[checkCounter].pid_nr_ns;
            t_eva_taskRank.state = ttaskRankData[checkCounter].state;
            t_eva_taskRank.priority = ttaskRankData[checkCounter].priority;
            t_eva_taskRank.task_policy = ttaskRankData[checkCounter].task_policy;
            t_eva_taskRank.task_cpu = ttaskRankData[checkCounter].task_cpu;
            t_eva_taskRank.taskOccupancy = ttaskRankData[checkCounter].taskOccupancy;
            t_eva_taskRank.utime = ttaskRankData[checkCounter].utime;
            t_eva_taskRank.stime = ttaskRankData[checkCounter].stime;
            strcpy(t_eva_taskRank.tcomm, ttaskRankData[checkCounter].tcomm);

            ttaskRankData[checkCounter].pid_nr_ns = ttaskRankData[checkCounter - 1].pid_nr_ns;
            ttaskRankData[checkCounter].state = ttaskRankData[checkCounter - 1].state;
            ttaskRankData[checkCounter].priority = ttaskRankData[checkCounter - 1].priority;
            ttaskRankData[checkCounter].task_policy = ttaskRankData[checkCounter - 1].task_policy;
            ttaskRankData[checkCounter].task_cpu = ttaskRankData[checkCounter - 1].task_cpu;
            ttaskRankData[checkCounter].taskOccupancy = ttaskRankData[checkCounter - 1].taskOccupancy;
            ttaskRankData[checkCounter].utime = ttaskRankData[checkCounter - 1].utime;
            ttaskRankData[checkCounter].stime = ttaskRankData[checkCounter - 1].stime;
            strcpy(ttaskRankData[checkCounter].tcomm, ttaskRankData[checkCounter - 1].tcomm);

            ttaskRankData[checkCounter - 1].pid_nr_ns = t_eva_taskRank.pid_nr_ns;
            ttaskRankData[checkCounter - 1].state = t_eva_taskRank.state;
            ttaskRankData[checkCounter - 1].priority = t_eva_taskRank.priority;
            ttaskRankData[checkCounter - 1].task_policy = t_eva_taskRank.task_policy;
            ttaskRankData[checkCounter - 1].task_cpu = t_eva_taskRank.task_cpu;
            ttaskRankData[checkCounter - 1].taskOccupancy = t_eva_taskRank.taskOccupancy;
            ttaskRankData[checkCounter - 1].utime = t_eva_taskRank.utime;
            ttaskRankData[checkCounter - 1].stime = t_eva_taskRank.stime;
            strcpy(ttaskRankData[checkCounter - 1].tcomm, t_eva_taskRank.tcomm);
          } else {
            break;
          }
        }
        if ((dtime >= TASK_STAT_THRESHOLD) && // %%CPU over only
            (getpid() != p->pid_nr_ns) && // except current process
            (strcmp (p->tcomm, "(NS_BackupMgr)"))) { // except "BackupMgr"
          if (p->task_policy == SCHED_FIFO) {
            if ((g_fifo_task_occ < dtime) && (g_fifo_show_timer == 0)) { // first time only
              g_fifo_task_pid = p->pid_nr_ns;
              g_fifo_task_occ = dtime;
              g_fifo_utime = tstat.utime;
              g_fifo_stime = tstat.stime;
              strcpy(g_fifo_path,p->path);
            } else if (g_fifo_task_pid == p->pid_nr_ns) {
              g_fifo_task_occ = dtime;
              g_fifo_utime = tstat.utime;
              g_fifo_stime = tstat.stime;
              strcpy(g_fifo_path,p->path);
            }
          } else {
            if ((g_tss_task_occ < dtime) && (g_tss_show_timer == 0)) { // first time only
              g_tss_task_pid = p->pid_nr_ns;
              g_tss_task_occ = dtime;
              g_tss_utime = tstat.utime;
              g_tss_stime = tstat.stime;
              strcpy(g_tss_path,p->path);
            } else if (g_tss_task_pid == p->pid_nr_ns) {
              g_tss_task_occ = dtime;
              g_tss_utime = tstat.utime;
              g_tss_stime = tstat.stime;
              strcpy(g_tss_path,p->path);
            }
          }
		}
	  }
	}
    p = p->next;
  }

  if (setRankDatanum != 0) {
    FRAMEWORKUNIFIEDLOG (ZONE_WARN, __FUNCTION__, "[CpuHighLoad]  PID  ST PRI POLICY cpuX  %%CPU (%%ut:%%st) comm");
    for (checkCounter = 0;
         checkCounter < (setRankDatanum < TASK_STAT_RANK_NUM ? setRankDatanum : TASK_STAT_RANK_NUM); checkCounter++) {
      FRAMEWORKUNIFIEDLOG (ZONE_WARN, __FUNCTION__, "[CpuHighLoad]%5d   %c %3d %5s  cpu%d %4lu (%2lu.%03lu:%2lu.%03lu) %s",
          ttaskRankData[checkCounter].pid_nr_ns,
          ttaskRankData[checkCounter].state,
          ttaskRankData[checkCounter].priority,
          ttaskRankData[checkCounter].task_policy == SCHED_FIFO ? "FIFO"
        : ttaskRankData[checkCounter].task_policy == SCHED_RR ? "RR" : "TSS",
          ttaskRankData[checkCounter].task_cpu,
          ttaskRankData[checkCounter].taskOccupancy,
          ttaskRankData[checkCounter].utime * 100 / ((endTime - g_startTime)/10),
          ttaskRankData[checkCounter].utime * 100 % ((endTime - g_startTime)/10),
          ttaskRankData[checkCounter].stime * 100 / ((endTime - g_startTime)/10),
          ttaskRankData[checkCounter].stime * 100 % ((endTime - g_startTime)/10),
          ttaskRankData[checkCounter].tcomm);
    }
    FRAMEWORKUNIFIEDLOG (ZONE_WARN, __FUNCTION__, "[CpuHighLoad]  -------------------------------------------------");
  }

}


static void
show_cpuload_onetask(int loadType) {
  int ret;
  TASK_STAT tstat;
  unsigned long dtime;
  struct timespec tp;
  unsigned int endTime = 0;
  char loadPath[64];
  unsigned long beforeUtime;
  unsigned long beforeStime;
  if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
    endTime = ((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));  // Unit is msec
  } else {
    endTime = (WTC_CPU_INTERVAL * 1000) + g_startTime;
  }
  if (endTime - g_startTime <= 0) {  // LCOV_EXCL_BR_LINE 200: (endTime - g_startTime) must be bigger than 0
    // LCOV_EXCL_START 200: (endTime - g_startTime) must be bigger than 0
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG (ZONE_WARN, __FUNCTION__, "[CpuHighLoad] Timer abnormality.");
    endTime = g_startTime + 1;
    // LCOV_EXCL_STOP
  }
  if (loadType == FIFO_TASK_SHOW) {  // LCOV_EXCL_BR_LINE 200: loadType can't be FIFO_TASK_SHOW
    // LCOV_EXCL_START 200: loadType can't be FIFO_TASK_SHOW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    strcpy(loadPath,g_fifo_path);
    beforeUtime = g_fifo_utime;
    beforeStime = g_fifo_stime;
    // LCOV_EXCL_STOP
  } else {
    strcpy(loadPath,g_tss_path);
    beforeUtime = g_tss_utime;
    beforeStime = g_tss_stime;
  }
  if (strlen(loadPath) > 0) {
    ret = get_task_stat(loadPath, &tstat);
    if (ret >= 0) {
      dtime = (((tstat.utime + tstat.stime) - (beforeUtime + beforeStime)) * 1000) / (endTime - g_startTime);
      if (tstat.task_policy == SCHED_FIFO) {
        g_fifo_task_occ = dtime;
        g_fifo_utime = tstat.utime;
        g_fifo_stime = tstat.stime;
      } else {
        g_tss_task_occ = dtime;
        g_tss_utime = tstat.utime;
        g_tss_stime = tstat.stime;
      }
    }
  }
}



//void logging_cpuload_custom(void) {
//  struct timespec req = { 1, 0 };  // 1sec
//
//  cleanup_task_stat();
//  get_current_task_stat();
//
//  nanosleep(&req, NULL);
//
//  show_cpuload();
//}
unsigned long
logging_cpuload_custom(int32_t tmode) {
  int32_t res = 0;
  if (tmode == CPU_TASK_INIT) {
    g_fifo_task_pid = -1;
    g_tss_task_pid = -1;
    g_fifo_task_occ = 0;
    g_tss_task_occ = 0;
    g_fifo_utime = 0;
    g_fifo_stime = 0;
    g_tss_utime = 0;
    g_tss_stime = 0;
    memset(g_fifo_path, 0x00, sizeof(g_fifo_path));
    memset(g_tss_path, 0x00, sizeof(g_tss_path));
    cleanup_task_stat();
    get_current_task_stat();
  } else if (tmode == CPU_TASK_SHOW_BF) {
    cleanup_task_stat();
    get_current_task_stat();
  } else if (tmode == CPU_TASK_SHOW_AF) {
    show_cpuload();
  } else if (tmode == FIFO_TASK_SHOW) {  // LCOV_EXCL_BR_LINE 200: tmode can't be FIFO_TASK_SHOW
    // LCOV_EXCL_START 200: tmode can't be FIFO_TASK_SHOW
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    show_cpuload_onetask(FIFO_TASK_SHOW);
    // LCOV_EXCL_STOP
  } else if (tmode == TSS_TASK_SHOW) {
    show_cpuload_onetask(TSS_TASK_SHOW);
  } else if (tmode == CPU_FIFO_TASK_GET_ID) {
    res = g_fifo_task_pid;
    g_fifo_show_timer = 0;
  } else if (tmode == CPU_TSS_TASK_GET_ID) {
    res = g_tss_task_pid;
    g_tss_show_timer = 0;
  } else if (tmode == CPU_FIFO_TASK_GET_OCCUPANCY) {
    g_fifo_show_timer = g_fifo_show_timer + WTC_CPU_INTERVAL;
    res = g_fifo_task_occ;
    g_fifo_task_occ = 0;
  } else if (tmode == CPU_TSS_TASK_GET_OCCUPANCY) {  // LCOV_EXCL_BR_LINE 200: tmode can't be other value
    g_tss_show_timer = g_tss_show_timer + WTC_CPU_INTERVAL;
    res = g_tss_task_occ;
    g_tss_task_occ = 0;
  }
  return res;
}

