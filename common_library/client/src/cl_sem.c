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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>

#include <native_service/cl_sem.h>
#include "cl_error.h"

#define CL_MILLI_TO_NANO(time)  ((time) * 1000000U)
#define CL_SEC_TO_NANO(time)  ((time) * 1000000000U)
#define CL_NANO_TO_SEC(time)  ((time) / 1000000000U)

static void cl_TimeoutCalc(struct timespec *ts, unsigned int timeout) {
  unsigned long long  nsec;

  clock_gettime(CLOCK_REALTIME, ts);
  nsec = (unsigned long long)ts->tv_nsec + (CL_MILLI_TO_NANO((unsigned long long)timeout));
  ts->tv_sec = ts->tv_sec + (time_t)CL_NANO_TO_SEC(nsec);
  ts->tv_nsec = (__syscall_slong_t)(nsec - CL_SEC_TO_NANO((unsigned long long)CL_NANO_TO_SEC(nsec)));
}

int CL_SemWait(sem_t *semid, unsigned int timeout) {
  struct timespec ts, tmp;
  int       ret;

  while (1) {
    cl_TimeoutCalc(&ts, timeout);
    if ((ret = sem_timedwait(semid, &ts)) != 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc
      // LCOV_EXCL_START 5: fail safe for libc
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno == ETIMEDOUT) {
        /* Check the time considering the case where the time has been changed by clock_settime/settimeofday */
        /* If the current time is much larger than the timeout specified time,
           the system assumes that the time has changed and re-executes the command.
           (A threshold of 100 is appropriate) */
        clock_gettime(CLOCK_REALTIME, &tmp);
        if (difftime(tmp.tv_sec, ts.tv_sec) > 100.0) {
          CL_DBG_PRINT("detect clock changed\n");
          continue;
        }
      }
    }
    // LCOV_EXCL_STOP 5: fail safe for libc
    break;
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  } // LCOV_EXCL_LINE 10: end line

  return ret;
}

