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
/// \ingroup  tag_NS_MessageCenter
/// \brief    Declares the internal methods of message center
///
///
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MC_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MC_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////////////////
/// GetData
/// Get the message data, this may be from a queue or shared memory.
/// Deletes the data based on the value of BOOL retain, if it is stored in shared memory.
///
/// \param [in] data
///          PVOID -  pointer to the data received from the McReceive Call
/// \param [in] to
///          PVOID -  pointer to the data to be received
/// \param [in] retain
///          BOOL -  TRUE to retain the data in shared memory
///            FALSE to release the data
///
/// \return EFrameworkunifiedStatus
///         EFrameworkunifiedStatus - status indicating success or failure
///
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetData(PVOID data, PVOID to, BOOL retain);

////////////////////////////////////////////////////////////////////////////////////////////
/// McLogTransmitLoggingMsgProcessed
///
/// \param [in] p_sysHdr
///          PVOID - Header of the received message. Passed as PVOID since Frameworkunified layer doesn't have
///          knowledge about the header.
/// \param [in] bMsgProcessed
///          BOOL -  TRUE - If any action was taken on the received message else FALSE
///
/// \return none
///
////////////////////////////////////////////////////////////////////////////////////////////
VOID McLogTransmitLoggingMsgProcessed(PVOID p_sysHdr, BOOL bMsgProcessed);

////////////////////////////////////////////////////////////////////////////////////////////
/// Get the session id.
///
/// \param [in] data
///          PVOID -  pointer to the data was received from the McReceive Call
///
/// \return UI_32
///         sessionId - session id
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 mcGetMsgSsessionId(PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// Gets the is type of sync from message header.
///
/// \param [in] pData
///         void* -  pointer to the data was received from the McReceive Call
///
/// \return BOOL
///         isTypeOfSync -
///       if message type is sync, return TRUE. or not return FALSE.
////////////////////////////////////////////////////////////////////////////////////////////
BOOL mcGetIsTypeOfSync(PVOID data);

#ifdef __cplusplus
}
#endif
#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGECENTER_INCLUDE_NS_MC_INTERNAL_H_
