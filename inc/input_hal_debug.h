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

#ifndef INC_INPUT_HAL_DEBUG_H_
#define INC_INPUT_HAL_DEBUG_H_

#include "input_hal_frameworkunifiedlog.h"

// #define INPUT_DEBUG

#ifdef INPUT_DEBUG
#define INPUT_DBG_LOG(zone_index, fmt, ...) \
  FRAMEWORKUNIFIEDLOG(zone_index, __FUNCTION__, fmt, ## __VA_ARGS__);
#else
#define INPUT_DBG_LOG(zone_index, fmt, ...)
#endif

/* Log output control flag */
#define INPUT_LOG_ENABLE_TRACE    /* Normal log(Trace)    */
#define INPUT_LOG_ENABLE_ERROR    /* Error log            */
#define INPUT_LOG_ENABLE_RETAIL   /* Debug log            */

#ifdef INPUT_LOG_ENABLE_TRACE
#define INPUT_LOG_TRACE(fmt, ...) \
  FRAMEWORKUNIFIEDLOG(ZONE_INFO, __FUNCTION__, fmt, ## __VA_ARGS__);
#else
#define INPUT_LOG_TRACE(fmt, ...)
#endif

#ifdef INPUT_LOG_ENABLE_ERROR
#define INPUT_ERROR_LOG(fmt, ...)  \
  FRAMEWORKUNIFIEDLOG(ZONE_ERR, __FUNCTION__, fmt, ## __VA_ARGS__);
#else
#define INPUT_ERROR_LOG(fmt, ...)
#endif

#ifdef INPUT_LOG_ENABLE_RETAIL
#define INPUT_RETAIL_LOG(fmt, ...) \
  FRAMEWORKUNIFIEDLOG(ZONE_HMI_DBG, __FUNCTION__, fmt, ## __VA_ARGS__);
#else
#define INPUT_RETAIL_LOG(fmt, ...)
#endif

#endif  // INC_INPUT_HAL_DEBUG_H_
