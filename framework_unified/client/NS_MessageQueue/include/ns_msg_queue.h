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
 * @file ns_msg_queue.h
 * @brief \~english TestFramework provides interface for implementing functional test classes.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MSG_QUEUE_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MSG_QUEUE_H_

#include <native_service/frameworkunified_types.h>


#ifdef __cplusplus
extern "C" {
#endif

/// \brief Enum that defines the value Q Types
typedef enum eQType {
  eQTypeReveiver,
  eQTypeSender,
  eQTypeInvld
} EQType;

// PASA CCR IPC Framework API

////////////////////////////////////////////////////////////////////////////////////////////
/// OpenReceiver
/// The opens a handle to a Receiver message queue.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to receiver messages on
///
/// \return HANDLE
///         handle - to a recevier's message queue
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE OpenReceiver(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// OpenReceiverNotBlocked
/// The opens a handle to a Receiver message queue. Operations on this queue
/// are non-blocking if queue is empty or full.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to receiver messages on
///
/// \return HANDLE
///         handle - to a recevier's message queue
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE OpenReceiverNotBlocked(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// OpenSyncReceiver
/// The opens a handle to a response Receiver message queue.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to receiver messages on
///
/// \return HANDLE
///         handle - to a recevier's message queue
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE openSyncReceiver(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// OpenSender
/// The opens a handle for sending messages to another message queue.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to send messages too
///
/// \return HANDLE
///         handle - senders message queue handle
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE OpenSender(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// openSyncSender
/// The opens a handle for sending response to another message queue.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to send messages too
///
/// \return HANDLE
///         handle - senders message queue handle
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE openSyncSender(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// openSenderZc
/// The opens a handle for sending response to another message queue.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to send messages too
///
/// \return HANDLE
///         handle - senders message queue handle
///     INVALID_HANDLE - Either message name is NULL or length is >= MAX_QUEUE_NAME_SIZE
///     INVALID_HANDLE - or an error occurred while opening a queue
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE openSenderZc(PCSTR name);

/////////////////////////////////////////
#include <pthread.h>
HANDLE OpenSenderChild(PCSTR name, pthread_t threadid);
/////////////////////////////////////////

/////////////////////////////////////////
EFrameworkunifiedStatus JoinChild(HANDLE hChildApp);
/////////////////////////////////////////

/////////////////////////////////////////
EFrameworkunifiedStatus GetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio);
/////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
/// SendMessage
/// The function sends a message to a queue.
///
/// \param [in] hMessage
///         Handle - handle to the sendMessage, OpenSender
/// \param [in] length
///          UI_32 -  length of the data to be sent
/// \param [in] data
///          const void* - pointer to the data that needs to be transmitted
///
/// \return EFrameworkunifiedaStatus
///         status - status of the message(error)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendMessage(HANDLE hMessage, UI_32 length, PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// SendMessageWithPriority
/// The function sends a message to a queue.
///
/// \param [in] hMessage
///         Handle - handle to the sendMessage, OpenSender
/// \param [in] length
///          UI_32 -  length of the data to be sent
/// \param [in] data
///          const void* - pointer to the data that needs to be transmitted
/// \param [in] priority
///          EFrameworkunifiedMessagePriorties - priority at which the message should be sent at!
///
/// \return EFrameworkunifiedaStatus
///         status - status of the message(error)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus SendMessageWithPriority(HANDLE hMessage, UI_32 length, PVOID data, EFrameworkunifiedMessagePriorties priority);

////////////////////////////////////////////////////////////////////////////////////////////
/// ReceiveMessage
/// The function blocks on a message queue waiting for data to be received.
///
/// \param [in] hMessage
///         HANDLE - handle to the recvMessage, OpenReceiver
/// \param [in] length
///          UI_32 -  length of the data buffer provided
/// \param [in] data
///          void* -  pointer to the data to be received
///
/// \return SI_32
///         length read - indicates the number of bytes that were read from the queue (eFrameworkunifiedStatusErrOther) error
////////////////////////////////////////////////////////////////////////////////////////////
SI_32  ReceiveMessage(HANDLE hMessage, UI_32 length, PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// CloseReceiver
/// The closes a connection to a Receiver message queue.
///
/// \param [in] hMessage
///         HANDLE - handle to the recvMessage that will be closed
///
/// \return EFrameworkunifiedaStatus
///         status - indicates if the close was successfully (eFrameworkunifiedStatusOK) or not (anything else)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CloseReceiver(HANDLE handle);

////////////////////////////////////////////////////////////////////////////////////////////
/// CloseReceiver
/// The closes a connection to a Receiver message queue.
///
/// \param [in] hMessage
///         HANDLE - handle to the recvMessage that will be closed
///
/// \return EFrameworkunifiedaStatus
///          status - indicates if the close was successfully (eFrameworkunifiedStatusOK) or not (anything else)
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus CloseSender(HANDLE handle);


////////////////////////////////////////////////////////////////////////////////////////////
/// Flush
/// Flush all data on HANDLE's Receiver message queue.
///
/// \param [in] hMessage
///         HANDLE - handle to the recvMessage queue that will be flushed
///
/// \return void
////////////////////////////////////////////////////////////////////////////////////////////
void Flush(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
/// GetQueueType
/// Get the handle's queue type.
///
/// \param [in] hMessage
///         HANDLE - handle to the Message queue
///
/// \return EQType
///     EQType - queue type of handle
////////////////////////////////////////////////////////////////////////////////////////////
EQType GetQueueType(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
//  Function : TranslateError
//  Translates global error variables into FW EFrameworkunifiedStatus
///
/// \param [in] error
///         int - error variable
///
/// \return EFrameworkunifiedStatus
///     EFrameworkunifiedStatus - frameworkunified error type
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus TranslateError(int error);

////////////////////////////////////////////////////////////////////////////////////////////
/// GetQueueName
/// This gets the name of the message queue
///
/// \param [in] hMessage
///         HANDLE - handle of the message queue
///                  (handle created with McOpenReceiver or McOpenSender)
///
/// \return PCSTR
///         name - name of the message queue if handle is valid else NULL
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR GetQueueName(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
/// GetQueueFD
/// This gets the fd of the message queue
///
/// \param [in] hMessage
///         HANDLE - handle of the message queue
///                  (handle created with McOpenReceiver or McOpenSender)
///
/// \return int
///         fd - fd of the message queue if handle is valid else -1
////////////////////////////////////////////////////////////////////////////////////////////
int GetQueueFD(HANDLE hMessage);

#ifdef __cplusplus
}
#endif

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MSG_QUEUE_H_

/** @}*/
/** @}*/
/** @}*/
