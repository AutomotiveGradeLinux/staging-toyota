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
 * @brief \~english APIs to Open/Close and Send/Receive on message queues and shared memory.
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
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __NATIVESERVICES_MESSAGE_CENTER_IF_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_MESSAGE_CENTER_IF_H__

#include <native_service/frameworkunified_types.h>
#include <native_service/ns_shared_mem_if.h>


#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McOpenReceiver
/// \~english @par Brief
///        Opens a handle to a Receiver message queue.
/// \~english @param[in] name
///        PCSTR - name of the message queue you want to receiver messages on
/// \~english @retval Handle to a recevier's message queue (If INVALID_HANDLE is returned its an error)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Size of message queue name string specified by argument(name) is larger then 20byte. [NULL]
///       - Message queue name specified by argument(name) is NULL. [NULL]
///       - Size of name normalised from argument(name) for message queue is larger then 15byte[16byte when starts with
///       "/"]. [NULL]
///       - Failed to create(malloc) buffer to storage name normalised from argument(name) for message queue. [NULL]
///       - Failed to get(mq_open) message queue. [NULL]
///       - Failed to create(malloc) buffer to storage management information of message queue. [NULL]
/// \~english @par Detail
///       Create a message queue whose name is specified by argument, and return a handle for receiving message. \n
///       The message queue created in this API is used to receive message, but not to send message. \n
///       You can use message queue in McOpenSender to send message.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McReceive, McClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenReceiver(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McOpenReceiverNotBlocked
/// \~english @par Brief
///        Opens a handle to a Receiver message queue. Operations on this queue are
///        non-blocking if queue is empty or full.
/// \~english @param[in] name
///        PCSTR - name of the message queue you want to receiver messages on
/// \~english @retval Handle to a recevier's message queue (If INVALID_HANDLE is returned its an error)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Size of message queue name string specified by argument(name) is larger then 20byte. [NULL]
///       - Message queue name specified by argument(name) is NULL. [NULL]
///       - Size of name normalised from argument(name) for message queue is larger then 15byte[16byte when starts with
///       "/"]. [NULL]
///       - Failed to create(malloc) buffer to storage name normalised from argument(name) for message queue. [NULL]
///       - Failed to get(mq_open) message queue. [NULL]
///       - Failed to create(malloc) buffer to storage management information of message queue. [NULL]
/// \~english @par Detail
///       Create a message queue whose name is specified by argument, and return a handle for receiving message. \n
///       The message queue created in this API is used to receive message, but not to send message. \n
///       You can use message queue in McOpenSender to send message.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McReceive, McClose, McOpenSenderNotBlocked
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenReceiverNotBlocked(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McOpenSyncReceiver
/// \~english @par Brief
///        Opens a handle to a response Receiver message queue.
/// \~english @param[in] name
///        PCSTR - name of the message queue you want to receiver messages on
/// \~english @retval Handle to a recevier's message queue (If INVALID_HANDLE is returned its an error)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Size of message queue name string specified by argument(name) is larger then 20byte. [NULL]
///       - Message queue name specified by argument(name) is NULL. [NULL]
///       - Size of name normalised from argument(name) for message queue(don't start with "/") is larger then 19byte.
///       [NULL]
///       - Size of name normalised from argument(name) for message queue(start with "/") is larger then 20byte. [NULL]
///       - Failed to create(malloc) buffer to storage name normalised from argument(name) for message queue. [NULL]
///       - Failed to get(mq_open) message queue. [NULL]
///       - Failed to create(malloc) buffer to storage management information of message queue. [NULL]
/// \~english @par Detail
///       Create a message queue whose name is specified by argument, and return a handle for receiving message
///       by synchronous communication.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///        When the application used the message queue acquired by this API, note the following.
///        - Use McCreateInvokerName to obtain the receive message queue handle to be passed to the parameter name.
///        - It can not be used transmission because it is for reception.
///          McOpenSender must be used for the message queue trasmission.
///        - Cannot be used other than for waiting for response during synchronous communication.
///          (because the maxmum number of messages in the queue is specified as 2)
///        - When a message queue name (name) that has not been normalized is specified, transmission falis unless 18 byte are specified.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McCreateInvokerName, McInvokeSync, McClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenSyncReceiver(PCSTR name);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McOpenSender
/// \~english @par Brief
///        The opens a handle for sending messages to another message queue.
/// \~english @param[in] name
///        PCSTR - name of the message queue you want to send messages too
/// \~english @retval Handle for sending messages to a queue (If INVALID_HANDLE is returned its an error)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Size of message queue name string specified by argument(name) is larger then 20byte. [NULL]
///       - Message queue name specified by argument(name) is NULL. [NULL]
///       - Size of name normalised from argument(name) for message queue is larger then 15byte[16byte when starts with
///       "/"]. [NULL]
///       - Failed to create(malloc) buffer to storage name normalised from argument(name) for message queue. [NULL]
///       - Failed to get(mq_open) message queue. [NULL]
///       - Failed to create(malloc) buffer to storage management information of message queue. [NULL]
/// \~english @par Detail
///       Create a message queue whose name is specified by argument, and return a handle for sending message. \n
///       The message queue created in this API is used to send message, but not to receive message. \n
///       You can use message queue in McOpenReceiver to receive message.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McSend, McClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenSender(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// The opens a handle for sending messages to another message queue.
/// operations on this queue are non-blocking if queue is empty or full.
///
/// \param [in] name
///         PCSTR - name of the message queue you want to send messages too
///
/// \return HANDLE
///         handle - for sending messages to a queue (If INVALID_HANDLE is returned its an error)
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenSenderNotBlocked(PCSTR name);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McOpenSyncSender
/// \~english @par Brief
///        The opens a handle for sending responce to another message queue.
/// \~english @param[in] name
///        PCSTR - name of the message queue you want to send messages too
/// \~english @retval Handle for sending response to a queue (If INVALID_HANDLE is returned its an error)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Size of message queue name string specified by argument(name) is larger then 20byte. [NULL]
///       - Message queue name specified by argument(name) is NULL. [NULL]
///       - Size of name normalised from argument(name) for message queue(don't start with "/") is larger then 19byte.
///       [NULL]
///       - Size of name normalised from argument(name) for message queue(start with "/") is larger then 20byte. [NULL]
///       - Failed to create(malloc) buffer to storage name normalised from argument(name) for message queue. [NULL]
///       - Failed to get(mq_open) message queue. [NULL]
///       - Failed to create(malloc) buffer to storage management information of message queue. [NULL]
/// \~english @par Detail
///       Create a message queue whose name is specified by argument, and return a handle for sending message. \n
///       The message queue created in this API is used to send message, but not to receive message. \n
///       You can use message queue in McOpenReceiver to receive message.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McSend, McClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McOpenSyncSender(PCSTR name);

/////////////////////////////////////////
#include <pthread.h>
HANDLE McOpenSenderChild(PCSTR name, pthread_t childid);
/////////////////////////////////////////

/////////////////////////////////////////
EFrameworkunifiedStatus McJoinChild(HANDLE hChildApp);
/////////////////////////////////////////

/////////////////////////////////////////
EFrameworkunifiedStatus McGetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio);
/////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McReceive
/// \~english @par Brief
///        Retrieves data from a message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the recvMessage, McOpenRecevier
/// \~english @param[out] source
///        PSTR - app that sent this message
/// \~english @param[out] cmd
///        UI_32* - command message that has been received.
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[out] data
///        PVOID - pointer to the data to be received
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf - Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName - Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail - Invalid handle of message queue for receiving message
/// \~english @par Preconditons
///        Message queue(McOpenReceiver etc.) for receiving message is created.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Data buffer specified by argument(data) for receiving message is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Size of data buffer specified by argument(length) for receiving message is 0. [eFrameworkunifiedStatusInvldBuf]
///       - Buffer to storage application name specified by argument(source) for sending message is NULL.
///       [eFrameworkunifiedStatusInvldQName]
///       - Buffer to storage commond ID of protocol on the service specified by argument(cmd) is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - Size of application name of sender getted from received message is lager then 20byte. [eFrameworkunifiedStatusInvldBuf]
///       - Handle specified by argument(hMessage) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified by argument(hMessage) is not appropriate(which is invalid). [eFrameworkunifiedStatusFail]
///       - Message queue type of handle specified by argument(hMessage) is not receiving type. [eFrameworkunifiedStatusFail]
///       - File descriptor for receiving message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when receiving message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for receiving message is invalid. [eFrameworkunifiedStatusInvldBufSize]
/// \~english @par Detail
///        Receives data from message queue specified by argument.
///        Sets application of sender(source), commond ID of protocol on the service(cmd), receiving data(data), and
///        return.
///        It will block until getting message from message queue.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///        Since data is exchanged only in the message queue, the data size that can be received is up to 4096 bytes.
/// \~english @par
///        The caller must guarantee the allocation of the area for the receive data buffer
///        and the consistency of the sizes set in the arguments.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McSend
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McReceive(HANDLE hMessage, PSTR source, UI_32 *cmd, UI_32 length, PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McReceiveWithSession
/// \~english @par Brief
///        Retrieves data from a message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the recvMessage, McOpenRecevier
/// \~english @param[out] source
///        PSTR - app that sent this message
/// \~english @param[out] cmd
///        UI_32* - command message that has been received.
/// \~english @param[out] sessionid
///        UI_32* - Session ID
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[out] data
///        PVOID - pointer to the data to be received
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf - Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName - Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail - Invalid handle of message queue for receiving message
/// \~english @par Preconditons
///        Message queue(McOpenReceiver etc.) for receiving message is created.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Buffer to storage session ID specified by argument(sessionid) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - Buffer to storage handle of message queue specified by argument(hMessage) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Data buffer specified by argument(data) for receiving message is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Size of data buffer specified by argument(length) for receiving message is 0. [eFrameworkunifiedStatusInvldBuf]
///       - Buffer to storage application name specified by argument(source) for sending message is NULL.
///       [eFrameworkunifiedStatusInvldQName]
///       - Buffer to storage commond ID of protocol on the service specified by argument(cmd) is NULL.
///       [eFrameworkunifiedStatusInvldParam]
///       - Size of application name of sender getted from received message is lager then 20byte. [eFrameworkunifiedStatusInvldBuf]
///       - Handle specified by argument(hMessage) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified by argument(hMessage) is not appropriate(which is invalid). [eFrameworkunifiedStatusFail]
///       - Message queue type of handle specified by argument(hMessage) is not receiving type. [eFrameworkunifiedStatusFail]
///       - File descriptor for receiving message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when receiving message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for receiving message is invalid. [eFrameworkunifiedStatusInvldBufSize]
/// \~english @par Detail
///        Receives data from message queue specified by argument.Sets application of sender(source),
///        commond ID of protocol on the service(cmd), receiving data(data), session id(sessionid), and return.
///        It will block until getting message from message queue.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///        The caller must guarantee the allocation of the area for the receive data buffer
///        and the consistency of the sizes set in the arguments.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McSendWithSession
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McReceiveWithSession(HANDLE hMessage, PSTR source, UI_32 *cmd,  UI_32 *sessionid,
                                UI_32 length, PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetLength
/// \~english @par Brief
///        Get the length of data that needs to be retrieved.
/// \~english @param[in] data
///        PVOID - pointer to the data was received from the McReceive Call
/// \~english @retval UI_32 indicates the number of bytes that in the message
/// \~english @par
///        if return is 0 or less then invalid data was passed.
/// \~english @par Preconditons
///        - Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///        - Data of received message(data) is NULL. [0]
/// \~english @par Detail
///        Return message data length specified by the header of received message. \n
///        Use this data length as the buffer size specified by McGetDataOfSize.
/// \~english @par
///        This API does not distinguish between error and no data.
/// \~english @par
///        The parameter data of this API must be the data obtained from the message queue by McReceive, etc.
///        (Since the data obtained from the message queue incudes header information,
///         this API acquires necessary iformation from the header)
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McReceive, McReceiveWithSession, McGetDataOfSize
////////////////////////////////////////////////////////////////////////////////////////////
UI_32 McGetLength(PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetDataPointer
/// \~english @par Brief
///        Get header address of data from received message.
/// \~english @param[in] data
///        PVOID - received message data
/// \~english @retval PVOID - data address of received message
/// \~english @par Preconditons
///        Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///        - Data of received message(data) is NULL. [NULL]
/// \~english @par Detail
///        Get header address of data from received message excepting the header and system information.
/// \~english @par
///        Please note the following points when using this API for application.
///        - Since the pointer acquired by this API is part of the received message passed as an argument to this API,
///          the pointer must not be released by specifying he address acquired by this API.
///        - The parameter data of this API must be the data obtained from the message queue by McReceive, etc.
///          (Since the data obtained from the message queue incudes header information,
///           this API acquires necessary iformation from the header)
///        - When the next message is received, the internally held data is discarded.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McReceive, McReceiveWithSession, McGetDataOfSize
////////////////////////////////////////////////////////////////////////////////////////////
PVOID McGetDataPointer(PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetDataOfSize
/// \~english @par Brief
///        Get the message data, this may be from a queue or shared memory where the maximum
///        size of the data should not exceed the maximum size passed in.
///        And deletes the data if it is stored in shared memory.
/// \~english @param[in] data
///        PVOID - pointer to the data was received from the McReceive Call
/// \~english @param[in] to
///        PVOID - pointer to the data to be received
/// \~english @param[in] uiSize
///        UI_32 - maximum size of the buffer to which the received data is copied
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK - success
/// \~english @retval eFrameworkunifiedStatusInvldBuf - invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldHandle - invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - invalid buffer size
/// \~english @retval eFrameworkunifiedStatusErrOther - invalid shared memory ID specified by received message
/// \~english @par Preconditons
///        - Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///         - Size specified by argument(uiSize) is smaller then size of received message data(data).
///         [eFrameworkunifiedStatusInvldBufSize]
///         - Data of received message(data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///         - Pointer to buffer(to) which is used to storage data getted from message is NULL. [eFrameworkunifiedStatusInvldBuf]
///         - Received message data specified by argument(data) is invalid. [eFrameworkunifiedStatusInvldHndlType]
///         - Failed to access(shm_open/fstat/mmap) the shared memory of received message data(data).
///         [eFrameworkunifiedStatusErrOther]
///         - Shared memory ID specified by received message is invalid. [FrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Get data from received message excepting the header and system information.
///        Delete data storaged in shared memory after getting the data successfully.
/// \~english @par
///        Please note the following points when using this API for application.
///        - In the case where he received message data becomes 0byte, there is no need to call this API,
///          and be careful not to allocate the memory for the received message data by 0byte.
///        - Error details related to shared memory access canno be detected.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McGetLength
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McGetDataOfSize(PVOID data, PVOID to, UI_32 uiSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetDataOfSizeWithSMRetain
/// \~english @par Brief
///        Get the message data, this may be from a queue or shared memory where the maximum
///        size of the data should not exceed the maximum size passed in.
///        Does not delete the data if it is stored in shared memory.
/// \~english @param[in] data
///        PVOID - pointer to the data was received from the McReceive Call
/// \~english @param[in] to
///        PVOID - pointer to the data to be received
/// \~english @param[in] uiSize
///        UI_32 - maximum size of the buffer to which the received data is copied
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK - success
/// \~english @retval eFrameworkunifiedStatusInvldBuf - invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldHandle - invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - invalid buffer size
/// \~english @retval eFrameworkunifiedStatusErrOther - invalid shared memory ID specified by received message
/// \~english @par Preconditons
///        - Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///         - Size specified by argument(uiSize) is smaller then size of received message data(data).
///         [eFrameworkunifiedStatusInvldBufSize]
///         - Data of received message(data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///         - Pointer to buffer(to) which is used to storage data getted from message is NULL. [eFrameworkunifiedStatusInvldBuf]
///         - Received message data specified by argument(data) is invalid. [eFrameworkunifiedStatusInvldHndlType]
///         - Failed to access(shm_open/fstat/mmap) the shared memory of received message data(data).
///         [eFrameworkunifiedStatusErrOther]
///         - Shared memory ID specified by received message is invalid. [FrameworkunifiedStatusErrOther]
/// \~english @par Detail
///        Get data from received message excepting the header and system information.
///        Does not delete data storaged in shared memory after getting the data successfully.
/// \~english @par
///        Please note the following points when using this API for application.
///        - In the case where he received message data becomes 0byte, there is no need to call this API,
///          and be careful not to allocate the memory for the received message data by 0byte.
///        - Error details related to shared memory access canno be detected.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
///        McReceive
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McGetDataOfSizeWithSMRetain(PVOID data, PVOID to, UI_32 uiSize);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetSysInfoData
/// \~english @par Brief
///        Gets the data from system info buffer from message header.
/// \~english @param[in] data
///        PVOID - pointer to the data was received from the McReceive Call
/// \~english @param[out] to
///        PVOID - pointer to the data to be received
/// \~english @retval EFrameworkunifiedStatus indicates if the close was successfully
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldBuf - Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @par Preconditons
///        Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Data of received message(data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Pointer to buffer(to) which is used to storage system info getted from message is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Received message data specified by argument(data) is invalid. [eFrameworkunifiedStatusInvldHndlType]
/// \~english @par Detail
///        Gets system info from received message, and set argument(to) with it. \n
///        If there is no system info in received message,and set argument(to) with "\0".
/// \~english @par
///        Please note the following points when using this API for application.
///        - The parameter data of this API must be the data obtained from the message queue by McReceive, etc.
///          (Since the data obtained from the message queue incudes header information,
///           this API acquires necessary iformation from the header)
///        - Since the system information is 64 bytes (including the null character), the area of 64 byte must be allocated in the argument to fo this API.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McReceive, McReceiveWithSession
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McGetSysInfoData(PVOID data, PVOID to);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McClearData
/// \~english @par Brief
///        Clears the data that may be pending for the a message in shared memory.
/// \~english @param[in] data
///        PVOID - pointer to the data was received from the McReceive Call
/// \~english @retval EFrameworkunifiedStatus indicates if the data was clear successfully
/// \~english @retval eFrameworkunifiedStatusOK - success
/// \~english @retval eFrameworkunifiedStatusInvldBuf - invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldParam - invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldID - invalid ID
/// \~english @retval eFrameworkunifiedStatusFail - other errors
/// \~english @par Preconditons
///        - Message is received by McReceive and so on.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///         - Data of received message(data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///         - Received message data specified by argument(data) is invalid. [eFrameworkunifiedStatusInvldParam]
///         - Shared memory ID in received message is invalid. [eFrameworkunifiedStatusInvldParam]
///         - Shared memory specified by ID in received message does not exist. [eFrameworkunifiedStatusInvldID]
///         - Failed to delete shared memory specified by ID in received message. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///         Delete data from received message.
///         And delete the received message data if it is stored in shared memory.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McReceive, McReceiveWithSession
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McClearData(PVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McSend
/// \~english @par Brief
///        Sends data to a message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSender
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @param[in] cmd
///        UI_32 - command id aka the message that's being sent
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[in] data
///        PCVOID - pointer to the data to be sent
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK - Success
/// \~english @retval eFrameworkunifiedStatusNullPointer - NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName - Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull - Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF I- nvalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail - Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther - other errors(Failed to open/allocate shared memory)
/// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
/// \~english @par Preconditons
///        - Message queue(McOpenSender) for sending message is created.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Message queue name of HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue name of HANDLE specified in the argument (hService) exceeds 20byte. [eFrameworkunifiedStatusInvldHandle]
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Application name of sender specified in the argument (source) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Application name of sender specified in the argument (source) exceeds 20byte. [eFrameworkunifiedStatusInvldQName]
///       - Sender data does not match sender data size(size is larger then 0, and data is NULL). [eFrameworkunifiedStatusInvldBuf]
///       - Message queue type of handle specified by argument(hMessage) is not sending type. [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when sending message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for sending message is invalid. [eFrameworkunifiedStatusInvldBufSize]
///       - Some errors occur, when sending message. [eFrameworkunifiedStatusFail]
///       - Failed to access(shm_open/ftruncate/mmap) the shared memory for sending message data. [eFrameworkunifiedStatusErrOther]
///       - Sending data size specified in the argument (length) is out of range. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       It is an asynchronous API. It provides function to send message by message queue. \n
///       Message queue name created by McOpenSender is the sender.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///       Command ID must be within the range PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD(0x10) to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD(0xFFFFFFF0).
///       Even if a command ID out of range is specified, a message is sent by the command ID (no erro occurs).
///       Command ID out of range are resered ID used for session connection, etc.
/// \~english @par
///       Please note the following points when using this API for application.
///       - The maximum transmission data size (ength) described above is a theoretical value,
///         and the maximum actually usable data size depends on the environment requirements.\n
///         The maximum value is the upper limit of the type (UNIT_MAX) minus the following size.\n
///         Message header section \n
///       - Operation cannot be guaranteed when the size of the allocated area indicated by the address
///         of the pointer (data) to the data buffer does not match the size specified by the size of the data buffer (length).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McSendWithSession, McReceive
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSend(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McSendWithSession
/// \~english @par Brief
///        Sends data to a message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSender
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @param[in] cmd
///        UI_32 - command id aka the message that's being sent
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[in] data
///        PCVOID - pointer to the data to be sent
/// \~english @param[in] sessionid
///        UI_32 - session id
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK - Success
/// \~english @retval eFrameworkunifiedStatusNullPointer - NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusInvldQName - Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull - Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail - Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther - other errors(Failed to open/allocate shared memory)
/// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
/// \~english @par Preconditons
///        - Message queue(McOpenSender) for sending message is created.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Message queue HANDLE in HANDLE specified in the argument (hService) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - HANDLE specified in the argument message queue HANDLE in (hService) is not appropriate (which is invalid).
///       [eFrameworkunifiedStatusInvldHandle]
///       - Application name of sender specified in the argument (source) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Application name of sender specified in the argument (source) exceeds 20byte. [eFrameworkunifiedStatusInvldQName]
///       - Sender data does not match sender data size(size is larger then 0, and data is NULL). [eFrameworkunifiedStatusInvldBuf]
///       - Message queue type of handle specified by argument(hMessage) is not sending type. [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue is full. [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor for sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when sending message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for sending message is invalid. [eFrameworkunifiedStatusInvldBufSize]
///       - Some errors occur, when sending message. [eFrameworkunifiedStatusFail]
///       - Failed to access(shm_open/ftruncate/mmap) the shared memory for sending message data. [eFrameworkunifiedStatusErrOther]
///       - Sending data size specified in the argument (length) is out of range. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///       It is an asynchronous API. It provides function to send message by message queue. \n
///       Message queue name created by McOpenSender is the sender.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///       Command ID must be within the range PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD(0x10) to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD(0xFFFFFFF0).
/// \~english @par
///        Please note the following points when using this API for application.
///        - McSend should be used if no sessionid is specified.
///        - To communicate with an application that uses NS Framework Dispatcher, use FrameworkunifiedSendMsg, etc.
///        - The maximum transmission data size (length) described above is a theoretical value,
///          and the maximum actually usable data size depends on the environment requirements.\n
///        - The maximum value is the upper limit of the type (UNIT_MAX) minus the following size.\n
///          Message header section\n
///        - Operation cannot be guaranteed when the size of the allocated area indicated by the address
///          of the pointer (data) to the data buffer does not match the size specified by the size of the data buffer (length).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McSend, McReceiveWithSession, FrameworkunifiedSendMsg
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSendWithSession(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data, UI_32 sessionid);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McSendWithPriority
/// \~english @par Brief
///        Sends data to a message queue, inserts based on priority given.
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSender
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @param[in] cmd
///        UI_32 - command id aka the message that's being sent
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[in] data
///        PCVOID - pointer to the data to be sent
/// \~english @param[in] sessionid
///        UI_32 - session id
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK(success)
/// \~english @retval eFrameworkunifiedStatusFail(anything else)
/// \~english @par Preconditons
///        none
/// \~english @par Change of internal status
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hMessage) or hMessage is NULL.
///       - It is an incorrect value(source) or hMessage is NULL.
///       - It is an incorrect value(data) or hMessage is NULL.
///       - It is an incorrect value(priority).
///       - It is an incorrect value(sessionid).
/// \~english @par Detail
///       Invoke callback function with arguments to send data to message queue based on priority given.\n
///       If callback function is null and expectedCalls is '0', using default function 'McSendWithPriority' to send
///       data to message queue based on priority given.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///       Command ID must be within the range PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD(0x10) to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD(0xFFFFFFF0).
/// \~english @par
///        Please note the following points when using this API for application.
///        - McSend should be used if no sessionid is specified.
///        - To communicate with an application that uses NS Framework Dispatcher, use FrameworkunifiedSendMsg, etc.
///        - The maximum transmission data size (length) described above is a theoretical value,
///          and the maximum actually usable data size depends on the environment requirements.\n
///          The maximum value is the upper limit of the type (UNIT_MAX) minus the following size.\n
///          Message header section\n
///        - Operation cannot be guaranteed when the size of the allocated area indicated by the address
///          of the pointer (data) to the data buffer does not match the size specified by the size of the data buffer (length).
/// \~english @par
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
//         McOpenSender, McSend, McSendWithSession, FrameworkunifiedSendMsg
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSendWithPriority(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data,
                              EFrameworkunifiedMessagePriorties priority, UI_32 sessionid);

// EFrameworkunifiedStatus McSendWithPriorityWithSession(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data,
// EFrameworkunifiedMessagePriorties priority, UI_32 sessionid);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McInvokeSync
/// \~english @par Brief
///        Synchronous Invoke. Send data to message queue, and Receive data from message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSender
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @param[in] cmd
///        UI_32 - command id aka the message that's being sent
/// \~english @param[in] msgLength
///        UI_32 - length of the data buffer provided
/// \~english @param[in] msgData
///        PCVOID - pointer to the data to be sent
/// \~english @param[in] sessionid
///        UI_32 - session ID
/// \~english @param[in] hRcvMessage
///        HANDLE - handle to the receive message queue, McOpenSyncReceiver
/// \~english @param[in] responseLength
///        UI_32 -  length of the responnce receive buffer provided
/// \~english @param[out] responseData
///        PVOID -  pointer to the data to be receive
/// \~english @param[out] receivedLength
///        UI_32* - length of the data to be receive
/// \~english @retval EFrameworkunifiedStatus indicates invoke response or message sent error or response receive error
/// \~english @retval eFrameworkunifiedStatusOK - success
/// \~english @retval eFrameworkunifiedStatusInvldParam - Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle - Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - Invalid handle type
/// \~english @retval eFrameworkunifiedStatusInvldQName - Invalid message queue name
/// \~english @retval eFrameworkunifiedStatusMsgQFull - Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - Invalid file descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - Invalid buffer size
/// \~english @retval eFrameworkunifiedStatusInvldBuf - Invalid buffer
/// \~english @retval eFrameworkunifiedStatusBadConnection - Can't connect with Socket
/// \~english @retval eFrameworkunifiedStatusFail - Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusErrOther - other errors(Failed to open/allocate shared memory)
/// \~english @par Preconditons
///       - Message queue specified by argument hMessage and hRcvMessage is useful.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - Message queue handle specified in the argument (hMessage) for sending message is NULL.
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue handle specified in the argument (hMessage) for sending message is not appropriate (which is
///       invalid). [eFrameworkunifiedStatusInvldHandle]
///       - Message queue handle specified in the argument (hRcvMessage) for receiving message is NULL.
///       [eFrameworkunifiedStatusInvldHandle]
///       - Message queue handle specified in the argument (hRcvMessage) for receiving message is not appropriate (which
///       is invalid). [eFrameworkunifiedStatusInvldHandle]
///       - Size of buffer(responseLength) which is used to storage received message is not 0,
///         but the pointer to this buffer(responseData) is NULL. [eFrameworkunifiedStatusInvldHandle]
///       - Failed to create buffer to storage the response message. [eFrameworkunifiedStatusFail]
///       - Failed to generete random number of the sequence ID for sending message. [eFrameworkunifiedStatusFail]
///       - Data size of sending message is not 0, but the message data(msgData) is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Application name of sender specified in the argument (source) is NULL. [eFrameworkunifiedStatusInvldQName]
///       - Application name of sender specified in the argument (source) exceeds 20byte. [eFrameworkunifiedStatusInvldQName]
///       - Name of message queue handle specified in the argument (hMessage) for sending message is NULL.
///       [eFrameworkunifiedStatusInvldHandle]
///       - Name of message queue handle specified in the argument (hMessage) for sending message exceeds 20byte.
///       [eFrameworkunifiedStatusInvldHandle]
///       - Size of sending message specified in the argument is not 0,
///         but the pointer to sending message is NULL. [eFrameworkunifiedStatusInvldBuf]
///       - Message queue type of handle specified by argument(hMessage) is not sending type. [eFrameworkunifiedStatusInvldHndlType]
///       - Message queue for sending message is full. [eFrameworkunifiedStatusMsgQFull]
///       - File descriptor of message queue for sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when sending message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for sending message is invalid. [eFrameworkunifiedStatusInvldBufSize]
///       - Some errors occur, when sending message. [eFrameworkunifiedStatusFail]
///       - Failed to access the shared memory for sending message data. [eFrameworkunifiedStatusErrOther]
///       - Failed to connect with the socket(socket/bind/connect) for checking the connection with application of
///       sender. [eFrameworkunifiedStatusFail]
///       - Failed to wait(epoll_create/epoll_ctl/epoll_wait) for receiving response message. [eFrameworkunifiedStatusFail]
///       - Failed to reconnect with the socket(socket/bind/connect) for checking the connection with application of
///       sender. [eFrameworkunifiedStatusBadConnection]
///       - Application name of sender in response message exceeds 20byte.[eFrameworkunifiedStatusInvldBuf]
///       - Message queue type of handle specified by argument(hMessage) is not receiving type.
///       [eFrameworkunifiedStatusInvldHndlType]
///       - File descriptor of message queue for receiving message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///       - An interrupt is generated by the system call (signal) when receiving message. [eFrameworkunifiedStatusErrNoEINTR]
///       - Size of buffer for receiving message is invalid. [eFrameworkunifiedStatusInvldBufSize]
///       - Received message is storaged in shared memory, but data size of shared memory is 0. [eFrameworkunifiedStatusErrOther]
///       - Failed to create buffer(calloc) to storage date getted from shared memory. [eFrameworkunifiedStatusFail]
///       - Failed to access(shm_open/fstat/mmap) the shared memory for receiving message data. [eFrameworkunifiedStatusErrOther]
///       - Data size of received message specified by argument(receivedLength) is NULL. [eFrameworkunifiedStatusInvldParam]
///       - Sending data size specified in the argument (msgLength) is out of range. [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        It provides synchronous communication for message queue between sender and receiver specified in the
///        argument. \n
///        As it is a synchronous function, after sending a message, the sender will wait until getting response form
///        receiver.
///        After receiving the response, set responseData with the received data and return.
/// \~english @par
///        This API provides the inter-process communication function by the message queue for the application
///        which does not use the inter-process communication by NS Framework Dispatcher.
/// \~english @par
///        When data larger than the receive message storage buffer size (resposeLength) is received,
///        the write size to the receive message storage buffer (responseData) does not exceed the specified size,
///        but the receive size is set in the receive message data length (recievedLength) as is.
/// \~english @par
///        The caller must guarantee the allocation of the receive message storage buffer (resonseData) area
///        and the consistency of the receive message stoage buffer sizes (resposeLength).
/// \~english @par
///        Please note the following points when using this API for application.
///        - This API stores up to the storage buffer length (responseLength) and returs eFrameworkunifiedStatusOK even
///          if the received reply message is larger than the storage buffer (responseData) size.\n
///          Therefore, the application shoud compare the received message length (recievedLenght) and the storage buffer length (responseLength) even
///          if this API returns eFrameworkunifiedStatusOK, and detect the above.
///        - Specify 0 for the sessionid of argument.
///          If a value other than 0 is specified, no error occurs and the value is used as is.
///        - To communicate with an application that uses NS Framework Dispatcher, use FrameworkunifiedInvokeSync, etc.
///        - The maximum data lenght (msgLength) of sent messages is a theoretical value,
///          and the maximum value that can be actually used is environment-depedent.\n
///          The maximum value is the upper limit of the type (UNIT_MAX) minus the following size.\n
///          Message header section, System information department in messages\n
///        - Operation cannot be guaranteed when the size of the allocated area indicated by the address
///          of the pointer (msgData) to the data buffer does not match the size specified by the size of the data buffer (msgLength).
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McOpenSyncReceiver, FrameworkunifiedInvokeSync
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McInvokeSync(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 msgLength, PCVOID msgData, UI_32 sessionid,
                        HANDLE hRcvMessage, UI_32 responseLength, PVOID responseData, UI_32 *receivedLength);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McSendSyncResponse
/// \~english @par Brief
///        Sends response to a message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSyncSender
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @param[in] cmd
///        UI_32 - command id aka the message that's being sent
/// \~english @param[in] seq_id
///        UI_32 - response sequence ID
/// \~english @param[in] ret_val
///        EFrameworkunifiedStatus - response status
/// \~english @param[in] length
///        UI_32 - length of the data buffer provided
/// \~english @param[in] data
///        PCVOID - pointer to the data to be sent
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle - If handle is NULL or invalid
/// \~english @retval eFrameworkunifiedStatusInvldHndlType -if handle type is invalid
/// \~english @retval eFrameworkunifiedStatusInvldQName - if MQ name is invalid
/// \~english @retval eFrameworkunifiedStatusMsgQFull - if MQ is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - if fd is invalid
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR - if system call(signal) interupt occurs
/// \~english @retval eFrameworkunifiedStatusInvldBufSize -if buffer size is invalid
/// \~english @retval eFrameworkunifiedStatusInvldBuf  -if buffer is invalid
/// \~english @retval eFrameworkunifiedStatusFail - any other error
/// \~english @retval eFrameworkunifiedStatusErrOther -other error(memory open/allocate fail)
/// \~english @par Preconditons
///       - The message queue specified by argument hMessage of this API must be allocated
///         in advance by McOpenSender and McOpenSyncSender.
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - sending MQ handle(designated by parameter)is NULL[eFrameworkunifiedStatusInvldHandle]
///       - sending MQ handle(designated by parameter)is invalid[eFrameworkunifiedStatusInvldHandle]
///       - the type of sending MQ handle(designated by parameter)is not sending type[eFrameworkunifiedStatusInvldHndlType]
///       - application name(source,designated by parameter)is NULL[eFrameworkunifiedStatusInvldQName]
///       - the length of application name(designated by parameter)is over 20byte[eFrameworkunifiedStatusInvldQName]
///       - sending data not in accordance with the size of sending data(eg:size is not 0 but data is NULL)
///       [eFrameworkunifiedStatusInvldBuf]
///       - sending data MQ is full [eFrameworkunifiedStatusMsgQFull]
///       - fd of sending data MQ is invalid [eFrameworkunifiedStatusErrNoEBADF]
///       - system call(signal) interupt occurs during MQ receiveing process[eFrameworkunifiedStatusErrNoEINTR]
///       - the size of receiveing MQ buffer is invalid[eFrameworkunifiedStatusInvldBufSize]
///       - error occurs during MQ receiveing process [eFrameworkunifiedStatusFail]
///       - failed to access shared memmory for sending message[eFrameworkunifiedStatusErrOther]
///       - failed to allocate buffer for data retriving from shared memmory[eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        set the response data to buffer and call this API
///        when need to response to synchronous message get from synchronous communication(McInvokeSync)
/// \~english @par
///        This API applies communication function(using message queue)between processes to the application which do not
///        use
///        Dispatcher of NS Framework
/// \~english @par
///        cautions when using this API
///        - Use FrameworkunifiedSetSyncResponseData if communacate with application which use the Dispatcher of NS Framework\n
///        - the limit of size for sending data is according to system
///          but the total size of SSystemMsgHeader and ResponseHeader shared memmory header is 48byte
///          size limit should be(UINT_MAX-48byte)
///        - operation is not guaranteed if the size which shown in data buffer pointer(data) is not in
///          accordance with the length set in data buffer
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McOpenSyncReceiver, McInvokeSync, FrameworkunifiedSetSyncResponseData
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McSendSyncResponse(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 seq_id, EFrameworkunifiedStatus ret_val, UI_32 length,
                              PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McCreateInvokerName
/// \~english @par Brief
///        Create synchronous invoke response MQ name.
/// \~english @param[in] source
///        PCSTR - source service name
/// \~english @param[in] sessionid
///        UI_32 - session id
/// \~english @param[in] invokerName
///        PSTR - create invoker name buffer
/// \~english @param[in] size
///        UI_32 - InvokerName buffer size
/// \~english @retval EFrameworkunifiedStatus indicates if the close was successfully
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusInvldParam -if parameter is invalid
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - sender application name(source,designated by parameter) is NULL[eFrameworkunifiedStatusInvldParam]
///       - synchronous communication MQ name(invokerName,designated by parameter) is NULL[eFrameworkunifiedStatusInvldParam]
///       - the size of synchronous communication MQ name buffer(designated by parameter)is NULL[eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        This API will add specific letters to the end of sender application name and return it as synchronous
///        communication MQ name
///        This name will be used when application create the MQ through McOpenSyncReceiver
/// \~english @par
///        cautions when using this API:
///        - the synchronous communication MQ name(invokerName) buffer size should be larger than the length of source
///        application
///          name(at least 4byte larger)
///        - source application name(designated by parameter) should be less than 15byte(according to NS framework
///        spec,error occurs
///          if message name length is over 20byte
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSyncReceiver
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McCreateInvokerName(PCSTR source, UI_32 sessionid, PSTR invokerName, UI_32 size);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McClose
/// \~english @par Brief
///        Close a connection to a Receiver message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle that will be closed
/// \~english @retval EFrameworkunifiedStatus indicates if the close was successfully
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle - If hMessage is NULL or invalid
/// \~english @retval eFrameworkunifiedStatusInvldHndlType - if hMessage  is invalid
/// \~english @par Preconditons
///           message queue(McOpenSender, McOpenReceiver) is created in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(hMessage,designated by parameter) is NULL[eFrameworkunifiedStatusInvldHandle]
///       - handle(hMessage,designated by parameter) is invalid[eFrameworkunifiedStatusInvldHandle]
///       - the type of handle(designated by parameter) is neither sending or receiveing[eFrameworkunifiedStatusInvldHndlType]
///       - fail to close the message queue(mq_close)[eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        close the message sending or receiveing message queue which is designated by parameter\n
///        Application can call this API to close the message queue which is created by McOpenSender
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McOpenReceiver, McOpenSyncReceiver
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McClose(HANDLE hMessage);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetDataUSID
/// \~english @par Brief
///        Get the USID is the Unique Shared Memory ID.e. from the message that was received.
/// \~english @param[in] pData
///        void* -  pointer to the data was received from the McReceive Call
/// \~english @retval TMemID Type of memory buffer identifiers(unsigned int).
/// \~english @retval NoneZero(Success)
/// \~english @retval Zero(Failed)
/// \~english @par Preconditons
///        none
/// \~english @par Change of internal status
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(pData) or pData is NULL.
/// \~english @par Detail
///       Invoke callback function with arguments to get the USID from the message that was received.\n
///       If callback function is null and expectedCalls is '0', using default function 'McGetDataUSID' to get the USID
///       from the message that was received.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
//         FrameworkunifiedGetDataUSID, FrameworkunifiedForwardMessage
//////////////////////////////////////////////////////////////////////////////////////////////
TMemID McGetDataUSID(PVOID pData);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetMsgSrc
/// \~english @par Brief
///        Gets the constant pointer to source message queue.
/// \~english @param[in] data
///        PVOID -  pointer to the data to be received
/// \~english @retval PCSTR message queue's address
///        CONST char* - constant pointer to source message queue
/// \~english @retval NotNull(Success)
/// \~english @retval Null(Failed)
/// \~english @par Preconditons
///        none
/// \~english @par Change of internal status
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(data) or data is NULL.
/// \~english @par Detail
///       Invoke callback function with arguments to get the constant pointer to source message queue.\n
///       If callback function is null and expectedCalls is '0', using default function 'McGetMsgSrc' to the constant
///       pointer to source message queue.\n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
//         frameworkunifiedSendSyncResponse, frameworkunifiedProcessSyncResponse
//////////////////////////////////////////////////////////////////////////////////////////////
PCSTR McGetMsgSrc(PVOID data);


//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McForward
/// \~english @par Brief
///        Forward a Message to indicate that there is a Shared Memory
/// \~english @param[in] hMessage
///        HANDLE - handle to the send message queue, McOpenSender
/// \~english @param[in] source
///        PCSTR -  the sender of this forwarding message.
/// \~english @param[in] iCmd
///        UI_32 - Command Id message to forward
/// \~english @param[in] USID
///        UI_32 - Unique Shared memory Id
/// \~english @retval EFrameworkunifiedStatus indicates if the close was successfully
///        CONST char* - constant pointer to source message queue
/// \~english @retval Success(eFrameworkunifiedStatusOK)
/// \~english @retval Failed(anything else)
/// \~english @par Preconditons
///        none
/// \~english @par Change of internal status
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(hMessage) or hMessage is NULL.
///       - It is an incorrect value(source) or source is NULL.
///       - It is an incorrect value(iCmd).
///       - It is an incorrect value(USID).
/// \~english @par Detail
///       Invoke callback function with arguments to forward a Message.\n
///       If callback function is null and expectedCalls is '0', using default function 'McForward' to forward a
///       Message.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
//         FrameworkunifiedForwardMessage
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McForward(HANDLE hMessage, PCSTR source, UI_32 iCmd, TMemID USID);


////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McFlushReceiver
/// \~english @par Brief
///        Flush's all data on HANDLE's Receiver message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle to the recvMessage queue that will be flushed
/// \~english @retval void
/// \~english @par Preconditons
///       message queue(McOpenReceiver)is created in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(hMessage,designated by parameter) is NULL
///       - handle(hMessage,designated by parameter) is invalid [-1]
///       - the type of handle(designated by parameter) is neither sending or receiveing[-1]
/// \~english @par Detail
///         message will not be flushed if process failed
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenSender, McOpenReceiver, McOpenSyncReceiver
////////////////////////////////////////////////////////////////////////////////////////////
void McFlushReceiver(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetQueueName
/// \~english @par Brief
///        Gets the name of the message queue.
/// \~english @param[in] hMessage
///         HANDLE - handle of the message queue,
///                  (handle created with McOpenReceiver or McOpenSender)
/// \~english @retval PCSTR name - name of the message queue if handle is valid else NULL
/// \~english @par Preconditons
///        Dispatcher for application should be created and initilized(FrameworkunifiedCreateDispatcherWithoutLoop)in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(hMessage,designated by parameter) is NULL
///       - handle(hMessage,designated by parameter) is invalid [-1]
///       - the type of handle(designated by parameter) is neither sending or receiveing[-1]
/// \~english @par Detail
///       return the name of specific message queue
///       this APL is used for application to supervize message queue
/// \~english @par
///        This API applies communication function(using message queue)between processes to the application which do not
///        use
///        Dispatcher of NS Framework
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McOpenSender
////////////////////////////////////////////////////////////////////////////////////////////
PCSTR McGetQueueName(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McGetQueueFD
/// \~english @par Brief
///        Gets the fd of the message queue.
/// \~english @param[in] hMessage
///        HANDLE - handle of the message queue.(handle created with McOpenReceiver or McOpenSender)
/// \~english @retval int fd - fd of the message queue if handle is valid else -1
/// \~english @par Preconditons
///        - Dispatcher for application is created and initialized(FrameworkunifiedCreateDispatcherWithoutLoop) in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(hMessage,designated by parameter) is NULL
///       - handle(hMessage,designated by parameter) is invalid [-1]
///       - the type of handle(designated by parameter) is neither sending or receiveing[-1]
/// \~english @par Detail
///       return the fd of specific message queue
///       this APL is used for application to supervize message queue
/// \~english @par
///        This API applies communication function(using message queue)between processes to the application which do not
///        use
///        Dispatcher of NS Framework
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McOpenReceiver, McOpenSender
////////////////////////////////////////////////////////////////////////////////////////////
int McGetQueueFD(HANDLE hMessage);

//////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McTranslateError
/// \~english @par Brief
///        Translates global error variables into FW EFrameworkunifiedStatus
/// \~english @param[in] error
///        int - error variable
/// \~english @retval EFrameworkunifiedStatus indicates frameworkunified error type
/// \~english @par
///        EFrameworkunifiedStatus Enum
/// \~english @code
///           typedef enum eFrameworkunifiedStatus {
///             eFrameworkunifiedStatusEmptyMediaList    = -10, ///< Empty media list
///             eFrameworkunifiedStatusSessionLimitMaxedOut  = -9,  ///< Maximum session limit reached
///             eFrameworkunifiedStatusDbRecNotFound       = -8, ///< Database record not found
///             eFrameworkunifiedStatusDbResultError       = -7, ///< Database result error
///             eFrameworkunifiedStatusDbExecuteFail       = -6, ///< Database execute fail
///             eFrameworkunifiedStatusSemCloseFail        = -5, ///< Semaphore close failed
///             eFrameworkunifiedStatusSemUnlockFail       = -4, ///< Semaphore unlock failed
///             eFrameworkunifiedStatusSemLockFail         = -3, ///< Semaphore lock failed
///             eFrameworkunifiedStatusFail                = -2, ///< Failed
///             eFrameworkunifiedStatusErrOther            = -1, ///< Unknown error
///             eFrameworkunifiedStatusOK                  = 0,  ///< Success / Pass / OK
///             eFrameworkunifiedStatusInvldBuf            = 1,  ///< Invalid buffer
///             eFrameworkunifiedStatusInvldHandle         = 2,  ///< Invalid handle
///             eFrameworkunifiedStatusInvldHndlType       = 3,  ///< Invalid handle type
///             eFrameworkunifiedStatusInvldQName        = 4,  ///< Invalid messasge queue name
///             eFrameworkunifiedStatusMsgQFull            = 5,  ///< Message queue full
///             eFrameworkunifiedStatusInvldNotification   = 6,///< The Notification event not present
///             eFrameworkunifiedStatusInvldParam        = 7,  ///< Invalid parameter
///             eFrameworkunifiedStatusInvldBufSize        = 8,  ///< Buf size too small
///             eFrameworkunifiedStatusInvldID             = 9,  ///< Unrecognized ID
///             eFrameworkunifiedStatusCannotRelease       = 10, ///< Cannot release resource
///             eFrameworkunifiedStatusBadConnection       = 11, ///< Could not locate resource
///             eFrameworkunifiedStatusExit            = 12,  ///< Normal application termination
///             eFrameworkunifiedStatusNotImplemented      = 13,  ///< incomplete feature
///             eFrameworkunifiedStatusThreadBusy          = 14,  ///< Joined thread is already being joined
///             eFrameworkunifiedStatusThreadSelfJoin      = 15,  ///< Thread is joining itself
///             eFrameworkunifiedStatusThreadInvalidVal    = 16, ///< Invalid value passed
///             eFrameworkunifiedStatusThreadNotExist      = 17,  ///< The thread does not exist
///             eFrameworkunifiedStatusFault               = 18,  ///< A fault occurred while attempting to make call
///             eFrameworkunifiedStatusServNotFound        = 19,  ///< Service not present in serv dir
///             eFrameworkunifiedStatusServerInUse         = 20,  ///< Service already processing 1 client request
///             eFrameworkunifiedStatusDbIndexing        = 21,  ///< Database Indexing in progress
///             eFrameworkunifiedStatusNullPointer       = 22,
///             eFrameworkunifiedStatusMsgNotProcessed     = 23,
///             eFrameworkunifiedStatusFileLoadSuccess     = 24, ///< File Load Success
///             eFrameworkunifiedStatusFileLoadError       = 25, ///< File Load Error
///             eFrameworkunifiedStatusAccessError         = 26, ///< Error when accessing resource
///             eFrameworkunifiedStatusDuplicate           = 27, ///< Duplicate entry
///             eFrameworkunifiedStatusMsgQEmpty             = 28, ///< Message queue empty
///             eFrameworkunifiedStatusThreadAlreadyRunning  = 29,
///             eFrameworkunifiedStatusErrNoEBADF            = 30,  ///< Bad file descriptor
///             eFrameworkunifiedStatusErrNoEAGAIN           = 31,  ///< Resource unavailable, try again
///             eFrameworkunifiedStatusErrNoEINTR            = 32,  ///< Interrupted system call
///             eFrameworkunifiedStatusSessionErr      = 33,  ///< Error in session handling
///             eFrameworkunifiedStatusDBCorrupt       = 34,  ///< Database corrupt
///             eFrameworkunifiedStatusDBFileNotFound    = 35  ///< Database file not found
///           } EFrameworkunifiedStatus, *PEFrameworkunifiedStatus;
///            @endcode
/// \~english @par Preconditons
///        none
/// \~english @par Change of internal status
///        - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - Function pointer specified by the argument is NULL.
///       - It is an incorrect value(error).
/// \~english @par Detail
///       Invoke callback function with arguments to translates global error variables.\n
///       If callback function is null and expectedCalls is '0', using default function 'McTranslateError' to translates
///       global error variables.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Sync only
/// \~english @see
//         none
//////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McTranslateError(int error);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McZcSetParam
/// \~english @par Brief
///        set variable parameter to messeg-sending header
/// \~english @param[in] handle
///        HANDLE - handle to the send message queue, McZcOpenSender
/// \~english @param[in] cmd
///        UI_32 -cmd for sending(ID to identify protocol of service)
/// \~english @param[in] length
///        UI_32 -size of buffer for message-sending
/// \~english @retval EFrameworkunifiedStatus indicates if the message was set successfully
/// \~english @retval eFrameworkunifiedStatusOK sucess
/// \~english @retval eFrameworkunifiedStatusInvldHandle - If hMessage is NULL or invalid
/// \~english @retval eFrameworkunifiedStatusInvldBuf - invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldBufSize - invalid buffer size
/// \~english @par Preconditons
///        message queue is created by McZcOpenSender in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(designated by parameter) is NULL[NULL]
///       - handle(designated by parameter) is invalid [NULL]
///       - the buffer for sending in handle(designated by parameter) is NULL[NULL]
///       - the size of data for sending(designated by parameter)(except header) is larger than the size of
///       sending-buffer[eFrameworkunifiedStatusInvldBufSize]
/// \~english @par Detail
///        set the variable sending cmd and sending data length in sending buffer of message-sending handle
///        This API will only do the initialization of message header.detail setting of data for sending will be done\n
///        Application can use McZcGetBuf to do data setting
/// \~english @par
///       CMD ID should be in (PROTOCOL_FRAMEWORKUNIFIED_BASE_CMD(0x10) to PROTOCOL_FRAMEWORKUNIFIED_MAX_CMD(0xFFFFFFF0))
/// \~english @par
///        cautions when using this API
///        - detail data sending size should be in accordance with the data sending size which is set in this API
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McZcGetBuf, McZcSend
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McZcSetParam(HANDLE handle, UI_32 cmd, UI_32 length);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McZcGetBuf
/// \~english @par Brief
///       get the fist address of buffer for sending
/// \~english @param[in] handle
///        HANDLE - handle to the send message queue, McZcOpenSender
/// \~english @retval PVOID  the first address of buffer for data-sending
/// \~english @retval NULL NULL pointer(fail to get)
/// \~english @par Preconditons
///        message queue is created by McZcOpenSender in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(designated by parameter) is NULL[NULL]
///       - handle(designated by parameter) is invalid [NULL]
///       - the buffer for sending in handle(designated by parameter) is NULL[NULL]
/// \~english @par Detail
///        get the first address of data(excpet the message-header)from sending-buffer in message-sending handle
///        application will set the address got as sending message
/// \~english @par
///        cautions When using the application
///        - result will not be guaranteed if  the handle set is different from message-sending queue handle
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McZcGetBuf
////////////////////////////////////////////////////////////////////////////////////////////
PVOID McZcGetBuf(HANDLE handle);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McZcSend
/// \~english @par Brief
///       send message
/// \~english @param[in] hMessage
///        HANDLE - handle to the sendMessage, McZcOpenSender
/// \~english @retval EFrameworkunifiedStatus indicates if the message was sent successfully
/// \~english @retval eFrameworkunifiedStatusOK success
/// \~english @retval eFrameworkunifiedStatusInvldHandle - If hMessage is NULL or invalid
/// \~english @retval eFrameworkunifiedStatusInvldBuf - buffer is invalid
/// \~english @retval eFrameworkunifiedStatusInvldHndlType -handle type is invalid
/// \~english @retval eFrameworkunifiedStatusMsgQFull - message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF - file descriptor invalid
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR -system call(signal) interupt
/// \~english @retval eFrameworkunifiedStatusInvldBufSize -invalid buffer size
/// \~english @par Preconditons
///        message queuqe is created by McZcOpenSender
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - handle(hMessage,designated by parameter)is NULL[eFrameworkunifiedStatusInvldHandle]
///       - handle(hMessage,designated by parameter)is invalid[eFrameworkunifiedStatusInvldHandle]
///       - the sending-buffer in handle(hMessage,designated by parameter)is NULL[eFrameworkunifiedStatusInvldBuf]
///       - the type of message queue in handle(hMessage,designated by parameter)is not sending
///       type[eFrameworkunifiedStatusInvldHndlType]
///       - message-sending queue is full[eFrameworkunifiedStatusMsgQFull]
///       - file descriptor of message-sending queue is invalid[eFrameworkunifiedStatusErrNoEBADF]
///       - system call(singal) interupts during the message sending process[eFrameworkunifiedStatusErrNoEINTR]
///       - the size of buffer for message-sending is invalid[eFrameworkunifiedStatusInvldBufSize]
/// \~english @par Detail
///        send message using specific message-sending handle.\n
///        data for sending should be set to message-sending handle(hMessage)beforhead using McZcSetParam, McZcGetBuf
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Method
/// \~english @see McZcOpenSender, McZcSetParam, McZcGetBuf
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McZcSend(HANDLE hMessage);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McZcOpenSender
/// \~english @par Brief
///        create message-sending queuqe
/// \~english @param[in] source
///        PCSTR - app (You) the sender
/// \~english @retval Handle for sending messages to a queue (If INVALID_HANDLE is returned its an error)
/// \~english @retval NULL NULL pointer(fail to get)
/// \~english @par Preconditons
///        - none
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - message queue name(designated by parameter)is NULL[NULL]
///       - the length of message queue name(designated by parameter)(source) is over 20byte[NULL]
///       - fail to get memmory for restoring formated message queue name [NULL]
///       - length of formated message queue name is over 15byte or 16byte(when initial letter is'/')[NULL]
///       - fail to get message queue(mq_open)[NULL]
///       - fail to get memmory for message queue info managment(malloc)[NULL]
///       - fail to get memory for sending buffer[NULL]
/// \~english @par Detail
///         create message queuqe and return the handle
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McZcSetParam, McZcGetBuf, McZcSend, McZcClose
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE McZcOpenSender(PCSTR source);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup McZcClose
/// \~english @par Brief
///       close the handle to message queue
/// \~english @param[in] handle
///        HANDLE - handle to the send message queue, handle got through McZcOpenSender
/// \~english @retval EFrameworkunifiedStatus indicates if the message was close successfully
/// \~english @retval eFrameworkunifiedStatusOK
/// \~english @retval eFrameworkunifiedStatusInvldHandle - If hMessage is NULL or invalid
/// \~english @retval eFrameworkunifiedStatusInvldHndlType If handle type is invalid
/// \~english @par Preconditons
///       - message queue is created by McZcOpenSender in advance
/// \~english @par Change of internal status
///        - none
/// \~english @par Conditions of processing failure
///       - message handle(designated by parameters) is NULL[eFrameworkunifiedStatusInvldHandle]
///       - message handle(designated by parameters) is invalid [eFrameworkunifiedStatusInvldHandle]
///       - message handle type(designated by parameters) is not match(neither send or receive)[eFrameworkunifiedStatusInvldHndlType]
///       - fail to close message queue(mq_close)[eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///        Close message-sending or message-receiving  handle(designated by parameters)\n
///        application call this function to close message queue which is created by McZcOpenSender.
/// \~english @par Classification
///          Public
/// \~english @par Type
///          Sync only
/// \~english @see McZcOpenSender
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus McZcClose(HANDLE handle);

#ifdef __cplusplus
}
#endif
#endif /* __NATIVESERVICES_MESSAGE_CENTER_IF_H__ */  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
