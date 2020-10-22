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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSLogger
/// \brief    API Header for Zone Player Service APIs to be used by senders and
///           receivers.
///
/// Declares the external APIs to Zone Player Service.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_NS_LOGGER_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_NS_LOGGER_INTERNAL_H_

// PASA Headers
#include <native_service/frameworkunified_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void _GetTimeString(PSTR format, UI_16 length);
void _BuildFormatString(HANDLE hMsgQ, const UI_16 f_uiZoneIndex, PCSTR process_name, PCSTR p_pstrClassName_i,
                        const UI_32 p_lLine_i, PCSTR lpszFormat, PSTR format, UI_16 length);

#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_LOGGER_INCLUDE_NS_LOGGER_INTERNAL_H_
