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
#ifndef HAL_API_CLOCK_HAL_H_
#define HAL_API_CLOCK_HAL_H_
/**
 * @file clock_hal.h
 */
#include <time.h>

#include <native_service/frameworkunified_types.h>
/** @addtogroup clock
 *  @{
 */
/** @addtogroup clock_hal
 *  @ingroup clock
 *  @{
 */
/**
 * \ingroup SetHardwareClock
 * \~english @par Brief
 *         API to set the RTC.
 * \~english @param [in] h_app
 *       HANDLE - Handle for Application
 * \~english @param [in] l_tm
 *       const struct tm * - pointer to the time will be set
 * \~english @retval eFrameworkunifiedStatusOK   :  Ok
 * \~english @retval eFrameworkunifiedStatusFail :  Failed
 * \~english @retval eFrameworkunifiedStatusInvldHandle : parameter "h_app" is NULL
 * \~english @retval eFrameworkunifiedStatusNullPointer : parameter "l_tm" is NULL
 * \~english @par Prerequisite
 *        - /dev/rtc node is exist, rtc driver is availability.
 * \~english @par Change of internal state
 *        - Change of internal state according to the API does not occur.
 * \~english @par Conditions of processing failure
 *        - Set time to RTC failed. [eFrameworkunifiedStatusFail]
 *        - The parameter "l_tm" is NULL. [eFrameworkunifiedStatusNullPointer]
 *        - The parameter "h_app" is NULL. [eFrameworkunifiedStatusInvldHandle]
 * \~english @par Detail
 *        - Set time to RTC.
 *        - The API can be used by 1 process.
 * \~english @par Classification
 *        Public
 * \~english @par Type
 *        Sync
 * \~english @see
 *        None
 */
EFrameworkunifiedStatus SetHardwareClock(HANDLE h_app, const struct tm *l_tm);


/**@}*/  // end of clock_hal
/**@}*/  // end of clock

#endif  // HAL_API_CLOCK_HAL_H_
