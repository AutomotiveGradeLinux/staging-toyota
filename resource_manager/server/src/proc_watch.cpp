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
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/vfs.h>    /* or <sys/statfs.h> */
#include <dirent.h>

//#include <soc_temperature_hal.h>
#include <cstring>
#include <cstdlib>
//#include <stub/ss_diag.h>

#include "ss_resm_resourcemanagerlog.h"
#include "proc_watch.h"
#include "resm_cfg.h"
#include "resm_internal.h"

#define WTC_TMPFS_INTERVAL (5)
#define WTC_MEM_INTERVAL (5)
#define DEBUG_INFO_OUTPUT_FREQ (1)

extern int isNfs;
extern void watchMem(void);
extern void outputResouceInfo(void);
extern EFrameworkunifiedStatus resourcemanagerlog_flag_check(UI_8 *mode);

#define _SOC_TEMP_MONITOR_

#if defined(_SOC_TEMP_MONITOR_)

extern void set_diag_ss_resourcemanager_err(uint16_t resourcemanager_code);

/*********************************************************************************
 * Soc Temperature
 register THSSR : T = CTEMP[5-0] x 5 - 65 [Celsius]
 *********************************************************************************/
#define SOC_TEMP_WATCH_INTVAL_TIMESEC (3 * 60)   // 3 minutes
// #define SOC_TEMP_WATCH_INTVAL_TIMESEC (1 * 5) // for debug
#define THSSR_TEMP_MASK 0x0000003F
#define TEMP_120 (120)
#define TEMP_125 (125)
#define TEMP_130 (130)

static float last_temp;
static int last_diag;

static void soc_temp_init(void) {
  FRAMEWORKUNIFIEDLOG(ZONE_SOC_TEMP, __FUNCTION__, "start");
  last_temp = 0.0f;
  last_diag = 0;
}

static void soc_temp(void) {
  float temp = 0.0f;
//  EFrameworkunifiedStatus e_status = GetSoCTemperature(&temp);
//  if (eFrameworkunifiedStatusOK != e_status) {
//    FRAMEWORKUNIFIEDLOG(ZONE_SOC_TEMP, __FUNCTION__,
//          "THSSR:GetSoCTemperature e_status(%d)", e_status);
//    return;
//  }

  if (TEMP_120 <= temp) {
    // FRAMEWORKUNIFIEDLOG
    FRAMEWORKUNIFIEDLOG(ZONE_SOC_TEMP, __FUNCTION__, "%f", temp);  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]

    // DIAGCODE
    if (TEMP_130 <= temp) {
      if (TEMP_130 != last_diag) {
#if 0  // TODO(Workaround to avoid build errors for customization by suppliers. Planned to fix on MK.)
        set_diag_ss_resourcemanager_err(DIAGCODE_VIDEOINHALEX_OVERTEMP_3); /* SoC Temperature anomaly(130 degree Or more) */
#endif
        last_diag = TEMP_130;
      }
    } else if (TEMP_125 <= temp) {
      if (TEMP_125 != last_diag) {
#if 0  // TODO(Workaround to avoid build errors for customization by suppliers. Planned to fix on MK.)
        set_diag_ss_resourcemanager_err(DIAGCODE_VIDEOINHALEX_OVERTEMP_2); /* SoC Temperature anomaly(125 degree Or more) */
#endif
        last_diag = TEMP_125;
      }
    } else {
      if (TEMP_120 != last_diag) {
#if 0  // TODO(Workaround to avoid build errors for customization by suppliers. Planned to fix on MK.)
        set_diag_ss_resourcemanager_err(DIAGCODE_VIDEOINHALEX_OVERTEMP_1); /* SoC Temperature anomaly(120 degree Or more) */
#endif
        last_diag = TEMP_120;
      }
    }
  } else {
    // FRAMEWORKUNIFIEDLOG
    if (TEMP_120 <= last_temp) {
      FRAMEWORKUNIFIEDLOG(ZONE_SOC_TEMP, __FUNCTION__, "%f", temp);  // LCOV_EXCL_BR_LINE 15: marco defined in "native_service/ns_logger_if.h"  // NOLINT[whitespace/line_length]
    }

    // DIAGCODE
    last_diag = 0;
  }
  last_temp = temp;
}
#endif  // defined ( _SOC_TEMP_MONITOR_ )

//
// Monitor /tmp
//
#define TMP_USED_SIZE (100*1024)  // 100MB
#define TMPFS_PATH "/tmp"
static int watch_tmpfs_enable = 1;

static void printdir(const char* path) {
  DIR *dirp;
  struct dirent entry, *next;
  char buf[512];

  dirp = opendir(path);
  if (dirp == NULL) {  // LCOV_EXCL_BR_LINE 5: opendir's error case.
    // LCOV_EXCL_START 5: opendir's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "opendir(%s) error.(%d)\n", path, errno);
    return;
    // LCOV_EXCL_STOP 5: opendir's error case.
  }
  while ((readdir_r(dirp, &entry, &next) == 0) && next != NULL) {
    if (((strlen(entry.d_name) == 1) && (strcmp(entry.d_name, ".") == 0))
        || ((strlen(entry.d_name) == 2) && (strcmp(entry.d_name, "..") == 0))) {
      continue;
    }
    if ((strlen(path) + strlen(entry.d_name) + 2) <= sizeof(buf)) {/* 2: '/','\0' */
      struct stat stat_buf;

      strcpy(buf, path);  // NOLINT
      strcat(buf, "/");  // NOLINT
      strcat(buf, entry.d_name);  // NOLINT
      if (stat(buf, &stat_buf) == -1) {  // LCOV_EXCL_BR_LINE 5: stat error case
        // LCOV_EXCL_START 5: stat error case.
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s stat error! (%d)\n", buf, errno);
        // LCOV_EXCL_STOP 5: stat's error case.
      } else if (S_ISDIR(stat_buf.st_mode)) {
        printdir(buf);
      } else {
        FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s %ld bytes", buf, stat_buf.st_size);
      }
    } else {
      FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "path(%s) name(%s) length over!\n", path,
             entry.d_name);
    }
  }
  closedir(dirp);

  return;
}

static void watchtmpfs(void) {
  int ret;
  struct statfs buf;
  long used;  // NOLINT

  if (!watch_tmpfs_enable) {
    return;
  }
  ret = statfs(TMPFS_PATH, &buf);
  if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: statfs error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return;  // LCOV_EXCL_LINE 5: statfs error case
  }
  used = (buf.f_blocks - buf.f_bavail) * buf.f_bsize / 1024;  // KB
  if (used > TMP_USED_SIZE) {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "%s Used %ld KB > %d KB",
    TMPFS_PATH,
           used, TMP_USED_SIZE);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "----- print %s start -----", TMPFS_PATH);
    printdir(TMPFS_PATH);
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, "----- print %s end -----", TMPFS_PATH);
    watch_tmpfs_enable = 0;
  }
}

/*********************************************************************************
 * proc_watch thread main
 *********************************************************************************/
void *
PRCW_main(void *p) {
  fd_set fds;
  int32_t maxFd = 0;
  int32_t timerFd;
  struct itimerspec tm;
  int32_t sec = 0;
  EFrameworkunifiedStatus flagRet;
  uint64_t exp;
  int ret;
  UI_8 mode;
#if defined(_SOC_TEMP_MONITOR_)
  int32_t timerFd2;
  struct itimerspec tm2;
#endif  // defined(_SOC_TEMP_MONITOR_)

  /* Memory monitoring and display debug information */
  timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (timerFd == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create error case
    // LCOV_EXCL_START 5: timerfd_create error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Create timerFd Failed. errno[%d]", errno);
    // LCOV_EXCL_STOP 5: timerfd_create error case
  } else {
    tm.it_value.tv_sec = MONITORING_START_DELAT_TIME;
    tm.it_value.tv_nsec = 0;
    tm.it_interval.tv_sec = 1;
    tm.it_interval.tv_nsec = 0;
    if (timerfd_settime(timerFd, 0, &tm, NULL) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create error case
      // LCOV_EXCL_START 5: timerfd_settime error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "[RESM_ERR]ResMgr Set timerFd Failed. errno[%d]", errno);
      // LCOV_EXCL_STOP 5: timerfd_settime error case
    }
  }

#if defined(_SOC_TEMP_MONITOR_)
  /* Soc temperature monitoring */
  timerFd2 = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (timerFd2 == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create error case
    // LCOV_EXCL_START 5: timerfd_create error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
           "[RESM_ERR]ResMgr Create timerFd2 Failed. errno[%d]", errno);
    // LCOV_EXCL_STOP 5: timerfd_create error case
  } else {
    tm2.it_value.tv_sec = 1;
    tm2.it_value.tv_nsec = 0;
    tm2.it_interval.tv_sec = SOC_TEMP_WATCH_INTVAL_TIMESEC;
    tm2.it_interval.tv_nsec = 0;
    if (timerfd_settime(timerFd2, 0, &tm2, NULL) == -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_settime error case
      // LCOV_EXCL_START 5: timerfd_settime error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__,
             "[RESM_ERR]ResMgr Set timerFd2 Failed. errno[%d]", errno);
      // LCOV_EXCL_STOP 5: timerfd_settime error case
    }
    soc_temp_init();
  }
#endif  // defined(_SOC_TEMP_MONITOR_)

  while (1) {
    FD_ZERO(&fds);
    if (timerFd != -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create's error case
      FD_SET(timerFd, &fds);
      maxFd = MY_MAX(maxFd, timerFd);
    }
#if defined(_SOC_TEMP_MONITOR_)
    if (timerFd2 != -1) {  // LCOV_EXCL_BR_LINE 5: timerfd_create's error case
      FD_SET(timerFd2, &fds);
      maxFd = MY_MAX(maxFd, timerFd2);
    }
#endif  // defined(_SOC_TEMP_MONITOR_)

    ret = select(maxFd + 1, &fds, NULL, NULL, NULL);
    if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: select error case
      // LCOV_EXCL_START 5: select error case
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno != EINTR) {
        FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "[RESM_ERR]ResMgr Handle Error");
        exit(EXIT_FAILURE);
      }
      continue;
      // LCOV_EXCL_STOP 5: select error case
    }

    if ((timerFd != -1) && FD_ISSET(timerFd, &fds)) {
      read(timerFd, &exp, sizeof(uint64_t));
      /* Memory monitoring */
      if ((sec % WTC_MEM_INTERVAL) == 0) {
        watchMem();
      }
      /* tmpfs monitoring */
      if (!isNfs && ((sec % WTC_TMPFS_INTERVAL) == 0)) {
        watchtmpfs();
      }
      /* Display debug information */
      if ((sec % DEBUG_INFO_OUTPUT_FREQ) == 0) {
        flagRet = resourcemanagerlog_flag_check(&mode);
        if ((flagRet == eFrameworkunifiedStatusOK) && (mode == FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG)) {
          outputResouceInfo();
        }
      }
      if (sec >= RESET_SEC) {
        sec = 0;
      }
      sec++;
    }
#if defined(_SOC_TEMP_MONITOR_)
    if ((timerFd2 != -1) && FD_ISSET(timerFd2, &fds)) {
      read(timerFd2, &exp, sizeof(uint64_t));
      soc_temp();
    }
#endif  // defined(_SOC_TEMP_MONITOR_)
  }
  return p;
}
