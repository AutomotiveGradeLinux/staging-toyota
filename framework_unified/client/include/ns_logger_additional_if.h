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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// Declares the external APIs to Logger.Include this header when you want to use FRAMEWORKUNIFIEDLOG_TEXT.
/// \ingroup  tag_NSLogger
/// @{
///
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_LOGGER_ADDITIONAL_IF_H_
#define  FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_LOGGER_ADDITIONAL_IF_H_

#include <native_service/ns_logger_if.h>

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    FRAMEWORKUNIFIEDLOG_TEXT
///////////////////////////////////////////////////////////////////////////////
/// FRAMEWORKUNIFIEDLOG_TEXT macro will output the log without relationship to the ZONE.
/// \note     Developers should use the FRAMEWORKUNIFIEDLOG for logging.
///////////////////////////////////////////////////////////////////////////////
#define FRAMEWORKUNIFIEDLOG_TEXT(funcname, args...) TEXT_ZONE(BITS_IN_ZONE_MASK, funcname, ## args)

#endif  // FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_LOGGER_ADDITIONAL_IF_H_
/// @}
