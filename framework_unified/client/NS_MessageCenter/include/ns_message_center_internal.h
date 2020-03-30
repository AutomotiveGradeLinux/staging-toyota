/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
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

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MESSAGE_CENTER_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MESSAGE_CENTER_INTERNAL_H_

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_shared_mem_if.h>


#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///  mcOpenMonitor
///  for Server API
///
/// \param [in] name
///         PCSTR - Service name
///
/// \return int
///     -1 - error
///     >0 - Listen socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcOpenMonitor(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcCloseMonitor
///  for Server API
///
/// \param [in] name
///         PCSTR - Service name
///
/// \return int
///     -1 - error
///      0 - success
////////////////////////////////////////////////////////////////////////////////////////////
int mcCloseMonitor(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcSetConnectKey
///  for Common API
///
/// \param [out] buf
///         char * - buffer
/// \param [in] size
///         int - buffer size
/// \param [in] serverName
///         PCSTR - Server name
/// \param [in] clientName
///         PCSTR - Client name
///
/// \return int
///     -1 - error
///     >0 - Connect socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcSetConnectKey(char *buf, size_t size, PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcConnectMonitor
///  for Client API
///
/// \param [in] serverName
///         PCSTR - server name
/// \param [in] clientName
///         PCSTR - client name
///
/// \return int
///     -1 - error
///     >0 - Connect socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcConnectMonitor(PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcCloseConnectMonitor
///  for Client API
///
/// \param [in] serverName
///         PCSTR - server name
/// \param [in] clientName
///         PCSTR - client name
///
/// \return int
///     -1 - error
///     >0 - Connect socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcCloseConnectMonitor(PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcAcceptMonitor
///  for Server API
///
/// \param [in] serverName
///         PCSTR - server name
/// \param [in] listenMonitorFd
///         int - Listen socket FD
///
/// \return int
///     -1 - error
///     >0 - Accept socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcAcceptMonitor(PCSTR serverName, int listenMonitorFd);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcGetAcceptMonitor
///  for Server API
///
/// \param [in] serverName
///         PCSTR - server name
/// \param [in] clientName
///         PCSTR - client name
///
/// \return int
///     -1 - error
///     >0 - Accept socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcGetAcceptMonitor(PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcCloseAcceptMonitor
///  for Server API
///
/// \param [in] serverName
///         PCSTR - server name
/// \param [in] clientName
///         PCSTR - client name
///
/// \return int
///     -1 - error
///     >0 - Accept socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcCloseAcceptMonitor(PCSTR serverName, PCSTR clientName);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcGetClientNameFromConnectKey
///  for Server API
///
/// \param [in] connectKey
///         PCSTR - connect key
/// \param [out] clientName
///         PCHAR - client name
/// \param [out] len
///         size_t - clientname buffer length
///
/// \return int
///     -1 - error
///     >0 - Accept socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcGetClientNameFromConnectKey(PCSTR connectKey, PCHAR clientName, size_t len);

////////////////////////////////////////////////////////////////////////////////////////////
///  mcGetServerNameFromConnectKey
///  for Client API
///
/// \param [in] connectKey
///         PCSTR - connect key
/// \param [out] serverName
///         PCHAR - client name
/// \param [out] len
///         size_t - serverName buffer length
///
/// \return int
///     -1 - error
///     >0 - Accept socket FD
////////////////////////////////////////////////////////////////////////////////////////////
int mcGetServerNameFromConnectKey(PCSTR connectKey, PCHAR serverName, size_t len);

#ifdef __cplusplus
}
#endif
#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MESSAGE_CENTER_INTERNAL_H_
