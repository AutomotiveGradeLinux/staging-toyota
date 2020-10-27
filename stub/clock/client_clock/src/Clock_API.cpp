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

/*
 * Information.
 * This source code is a sample source code .
 * Implementation of the function must be performed by the vendor.
 */

#include <stub/Clock_API.h>

/* static variable definition */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 function prototype
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CLOCK_RETURN Clock_getSystemTime(struct timespec* time, uint8_t* status) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_getLocalTime(const time_t* base_time, struct tm* local_time) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_CnvSecToDate(const time_t* timep, struct tm *result) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_CnvDateToSec(const struct tm* tm, time_t* result) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_setUserTime(const time_t* user_time) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_getUserTime(time_t* user_time, uint8_t* status) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_addUserTimeOffset(const struct tm* offset) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_getSystemTimeY2K38(uint32_t* time, uint8_t* status) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_getLocalTimeY2K38(const uint32_t* base_time, struct tm* local_time) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_CnvSecToDateY2K38(const uint32_t* time, struct tm *result) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}

CLOCK_RETURN Clock_CnvDateToSecY2K38(const struct tm* tm, uint32_t* result) {
  /*
   *  Note.
   *  This feature needs to be implemented by the vendor.
   */
    return CLOCK_OK;
}
