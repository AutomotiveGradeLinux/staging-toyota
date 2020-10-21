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

/**
 * @file soc_temperature_hal.h
 */

#ifndef HAL_API_SOC_TEMPERATURE_HAL_H_
#define HAL_API_SOC_TEMPERATURE_HAL_H_

#include <native_service/frameworkunified_types.h>

/** @addtogroup soc_temperature_hal
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * \ingroup GetSoCTemperature
 * \~english @par Brief
 *           Get SoC temperature value(unit:celsius).
 * \~english @param [out] temperature
 *      float*   -    degree celsius
 * \~english @retval eFrameworkunifiedStatusOK  Success
 * \~english @retval eFrameworkunifiedStatusNullPointer  NULL pointer specified
 * \~english @retval eFrameworkunifiedStatusFail  Abnormal Error (system error)
 * \~english @par Prerequisite
 *        - The SoC has temperature sensor, and the thermal driver is available.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Parameter temperature is NULL[eFrameworkunifiedStatusNullPointer]
 *        - System error(open/read/close failed)[eFrameworkunifiedStatusFail]
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @par Detail
 *        - Get SoC temperature value.
 *        - On success eFrameworkunifiedStatusOK is returned and the parameter temperature is set.
 *        - Not eFrameworkunifiedStatusOK is returned if an error occurred.
 *        - Return value shall be checked before temperature is refered.
 *        - Sample code:
 *            @code
 *            #include <stdio.h>
 *            #include <stdlib.h>
 *            #include <soc_temperature_hal.h>
 *            #include <native_service/frameworkunified_types.h>
 *
 *            int main(int argc, char *argv[]) {
 *              float temperature = 0.0f;
 *              EFrameworkunifiedStatus status = GetSoCTemperature(&temperature);
 *              if (status != eFrameworkunifiedStatusOK) {
 *                printf("Get SoC temperature failed\n");
 *                exit(EXIT_FAILURE);
 *              }
 *              printf("SoC temperature: %f\n", temperature);
 *              exit(EXIT_SUCCESS);
 *            }
 *            @endcode
 * \~english @see
 *        - None
 */
EFrameworkunifiedStatus GetSoCTemperature(float* temperature);

#ifdef __cplusplus
}
#endif  // __cplusplus

/** @}*/  // end of soc_temperature_hal

#endif  // HAL_API_SOC_TEMPERATURE_HAL_H_

