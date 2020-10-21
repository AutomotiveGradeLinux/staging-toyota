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
#include "soc_temperature_hal.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "soc_temperature_hal_soctemperaturehallog.h"

#define MAX_TEMPERATURE_FROM_DRIVER_LENGTH (9)  // 9bytes can content -999999\n\0 - 9999999\n\0 (unit:millicelsius)
#define DECIMAL  (10)
#define THERMAL_ZONE0_TEMPERATURE_PATH "/sys/class/thermal/thermal_zone0/temp"
#define THERMAL_ZONE1_TEMPERATURE_PATH "/sys/class/thermal/thermal_zone1/temp"
#define THERMAL_ZONE2_TEMPERATURE_PATH "/sys/class/thermal/thermal_zone2/temp"

EFrameworkunifiedStatus GetCoreTemperature(int64_t* temperature, const char* node_path) {
  if ((NULL == temperature) || (NULL == node_path)) {
    return eFrameworkunifiedStatusNullPointer;
  }

  char sz_temperature[MAX_TEMPERATURE_FROM_DRIVER_LENGTH] = { 0 };

  // read temperature
  int fd = open(node_path, O_RDONLY);
  if (0 > fd) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "open error(%d,%s)", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  ssize_t ret = read(fd, sz_temperature, MAX_TEMPERATURE_FROM_DRIVER_LENGTH - 1);
  int read_error = errno;
  close(fd);
  if (-1 == ret) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "read error(%d,%s)", read_error, strerror(read_error));
    return eFrameworkunifiedStatusFail;
  }

  errno = 0;
  int64_t temp = strtol(sz_temperature, NULL, DECIMAL);
  if (errno) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "strtol error(%d,%s)", errno, strerror(errno));
    return eFrameworkunifiedStatusFail;
  }

  *temperature = temp;
  return eFrameworkunifiedStatusOK;
}

// Get SoC temperature value.
EFrameworkunifiedStatus GetSoCTemperature(float *temperature) {
  if (NULL == temperature) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "parameter *temperature is null");
    return eFrameworkunifiedStatusNullPointer;
  }

  // Get temperature from thermal_zone0
  int64_t temperature0 = 0;
  EFrameworkunifiedStatus e_status = GetCoreTemperature(&temperature0, THERMAL_ZONE0_TEMPERATURE_PATH);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "get temperature0 failed");
    return eFrameworkunifiedStatusFail;
  }

  // Get temperature from thermal_zone1
  int64_t temperature1 = 0;
  e_status = GetCoreTemperature(&temperature1, THERMAL_ZONE1_TEMPERATURE_PATH);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "get temperature1 failed");
    return eFrameworkunifiedStatusFail;
  }

  // Get temperature from thermal_zone2
  int64_t temperature2 = 0;
  e_status = GetCoreTemperature(&temperature2, THERMAL_ZONE2_TEMPERATURE_PATH);
  if (eFrameworkunifiedStatusOK != e_status) {
    FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, "get temperature2 failed");
    return eFrameworkunifiedStatusFail;
  }

  // get max temperature
  int64_t max_temperature = temperature0;
  if (max_temperature < temperature1) {
    max_temperature = temperature1;
  }
  if (max_temperature < temperature2) {
    max_temperature = temperature2;
  }

  *temperature = max_temperature / 1000.0f;
  return eFrameworkunifiedStatusOK;
}
