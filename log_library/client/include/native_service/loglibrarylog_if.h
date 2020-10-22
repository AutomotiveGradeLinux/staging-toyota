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
/**
 * @file
 * @brief \~english This file contains defines and LOGLIBRARYLOG_Freeze API for free frameworkunifiedlog.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup log_library
 *  @ingroup native_service
 *  @{
 */

#ifndef __LOGLIBRARYLOG_IF_H__  // NOLINT  (readability/nolint)
#define __LOGLIBRARYLOG_IF_H__

#define FRAMEWORKUNIFIEDLOG_SHARED_MEM_NAME "loglibrarylogconfig.cfg"
#define FRAMEWORKUNIFIEDLOG_RAMDISC_NAME "/ramd/BS/ss/logger_service/rwdata/frameworkunifiedlog/frameworkunified_debug.log"
#define FRAMEWORKUNIFIEDLOG_RAMDISC_PATH "/ramd/BS/ss/logger_service/rwdata/frameworkunifiedlog"

// Out put mode
#define FRAMEWORKUNIFIEDLOG_FLAG_MODE_DEBUG    1  // Debug
#define FRAMEWORKUNIFIEDLOG_FLAG_MODE_RELEASE  0  // :Release

// RealtimeLog mode
#define LOGLIBRARYLOG_REALTIMELOG_DISABLE_MASK     0x80
#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_FREEZE      0xFF  // : FREEZE

#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_OFF         0  // :ALL OFF
#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_UART        1  // :UARTOUT
#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB         2  // :USB OUT
#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB_DISABLE \
    (FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_USB | LOGLIBRARYLOG_REALTIMELOG_DISABLE_MASK)
#define FRAMEWORKUNIFIEDLOG_REALTIMELOG_MODE_MAX         3  // :MAX NUM

//////////////////////////////////////////////////////////////////////////////////////
/// \ingroup LOGLIBRARYLOG_Freeze
/// \~english @par Brief
///       Freeze Loglibrarylog API.
/// \~english @retval 0: success
/// \~english @retval -1: error
/// \~english @par Prerequisite
///       - none
/// \~english @par Conditions of processing failure
///       - none
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       This API is provide to freeze Loglibrarylog.
/// \~english @see none
////////////////////////////////////////////////////////////////////////////////
int LOGLIBRARYLOG_Freeze(void);

#endif  // __LOGLIBRARYLOG_IF_H__  // NOLINT  (readability/nolint)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
