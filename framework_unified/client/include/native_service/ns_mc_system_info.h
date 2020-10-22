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
/// \ingroup  tag_NSMessageCenter
/// \brief    Native Services Special Interfaces
///
///
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_MC_SYSTEM_INFO_H_
#define FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_MC_SYSTEM_INFO_H_


#include <native_service/ns_msgs.h>

#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSMessageCenter
/// McSendWithSysInfo
/// Sends data to a message queue, user defined system info.
///
/// \param [in] hMessage
///         HANDLE - handle to the send message queue, McOpenSender
/// \param [in] source
///     LPCSTR - app (You) the sender
/// \param [in] cmd
///     UI_32 - command id aka the message that's being sent
/// \param [in] sysInfo
///          PCHAR -  pointer to sysInfo data that will be placed in the header of the message.
/// \param [in] length
///          UI_32 -  length of the data buffer provided
/// \param [in] data
///          void* -  pointer to the data to be sent
///
/// \return EFrameworkunifiedStatus
///         status -
///       indicates if the message was sent successfully
///       success >> (eFrameworkunifiedStatusOK)
///       failure >> (anything else)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSendWithSysInfo(HANDLE hMessage, PCSTR source, UI_32 cmd, PCHAR sysInfo, UI_32 length, PCVOID data,
                             UI_32 sessionid);

// EFrameworkunifiedStatus McSendWithSysInfoWithSession(HANDLE hMessage, PCSTR source,
// UI_32 cmd, PCHAR sysInfo, UI_32 length, PCVOID data, UI_32 sessionid);




////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup tag_NSMessageCenter
/// McSendWithSysInfo
/// Sends data to a message queue, user defined system info.
///
/// \param [in] hMessage
///         HANDLE - handle to the send message queue, McOpenSender
/// \param [in] source
///     LPCSTR - app (You) the sender
/// \param [in] cmd
///     UI_32 - command id aka the message that's being sent
/// \param [in] sysInfo
///          PCHAR -  pointer to sysInfo data that will be placed in the header of the message.
/// \param [in] length
///          UI_32 -  length of the data buffer provided
/// \param [in] data
///          void* -  pointer to the data to be sent
/// \param [in] priority
///          EFrameworkunifiedMessagePriorties - various Message Priorities that a process can assign.
///
/// \return EFrameworkunifiedStatus
///         status -
///       indicates if the message was sent successfully
///       success >> (eFrameworkunifiedStatusOK)
///       failure >> (anything else)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSendPrioWithSysInfo(HANDLE hMessage, PCSTR source, UI_32 cmd, PCSTR sysInfo, UI_32 length, PCVOID data,
                                 EFrameworkunifiedMessagePriorties priority, UI_32 sessionid);

// EFrameworkunifiedStatus McSendPrioWithSysInfoWithSession(HANDLE hMessage, PCSTR source,
// UI_32 cmd, PCSTR sysInfo, UI_32 length, PCVOID data, EFrameworkunifiedMessagePriorties priority, UI_32 sessionid);

#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_INCLUDE_NS_MC_SYSTEM_INFO_H_
