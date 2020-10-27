/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
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
#include "clock_hal.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "clock_hal_clockhallog.h"

#define RTC_NODE_PATH_DEPTH 64
#define RTC_NODE_PERANT_PATH "/dev"

// set time to hardware
EFrameworkunifiedStatus SetHardwareClock(HANDLE h_app, const struct tm *l_tm) {
  if (NULL == h_app) {
    return eFrameworkunifiedStatusInvldHandle;
  }
  if (NULL == l_tm) {
    return eFrameworkunifiedStatusNullPointer;
  }

  EFrameworkunifiedStatus e_status = eFrameworkunifiedStatusOK;
  struct rtc_time hal_rtc_time;
  hal_rtc_time.tm_year = l_tm->tm_year;
  hal_rtc_time.tm_mon = l_tm->tm_mon;
  hal_rtc_time.tm_mday = l_tm->tm_mday;
  hal_rtc_time.tm_hour = l_tm->tm_hour;
  hal_rtc_time.tm_min = l_tm->tm_min;
  hal_rtc_time.tm_sec = l_tm->tm_sec;
  hal_rtc_time.tm_wday = l_tm->tm_wday;
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
         "setting time : %d-%02d/%02d %02d:%02d:%02d\n",
         1900 + l_tm->tm_year, l_tm->tm_mon + 1, l_tm->tm_mday,
         l_tm->tm_hour, l_tm->tm_min, l_tm->tm_sec);

  // Search RTC node
  DIR *dir = ::opendir(RTC_NODE_PERANT_PATH);
  char rtc_path[RTC_NODE_PATH_DEPTH] = { '\0' };
  for (struct dirent *dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
    if (strstr(dp->d_name, "rtc") != NULL) {
      snprintf(rtc_path, sizeof(rtc_path), "/dev/%s", dp->d_name);
      break;
    }
  }
  closedir(dir);
  if (strlen(rtc_path) == 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "/dev/rtcX not found.\n");
    e_status = eFrameworkunifiedStatusFail;
    return e_status;
  }

  // Set time to /dev/rtcX
  int fd = ::open(rtc_path, O_WRONLY);
  if (-1 == fd) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__, "Error open(/dev/rtc)=[%d] errno=%d\n",
           fd, errno);
    return eFrameworkunifiedStatusFail;
  }
  int ret = ::ioctl(fd, RTC_SET_TIME, &hal_rtc_time);
  if (0 != ret) {
    FRAMEWORKUNIFIEDLOG(ZONE_WARN, __FUNCTION__,
           "Error ioctl(fd, RTC_SET_TIME, hal_rtc_time)=[%d] errno=%d\n",
           ret, errno);
    e_status = eFrameworkunifiedStatusFail;
  } else {
    FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__,
           "Success ioctl(fd, RTC_SET_TIME, hal_rtc_time)=[%d]\n", ret);
  }
  ::close(fd);
  return e_status;
}
