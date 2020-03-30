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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSFramework
/// \brief    Application framework's core structures and data types.
///
///
///
///////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file frameworkunified_multithreading.h
 * @brief \~english Application framework's core structures and data types.
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
/** @addtogroup framework
 *  @ingroup native_service
 *  @{
 */
#ifndef __NSFRAMEWORK_NFRAMEWORKCORE_MULTITHREADING__  // NOLINT  (build/header_guard)
#define __NSFRAMEWORK_NFRAMEWORKCORE_MULTITHREADING__

#include <native_service/frameworkunified_framework_types.h>

#define INHERIT_PARENT_THREAD_PRIO -100

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateChildThread
/// \~english @par Brief
///        Create child thread and dispatcher for child thread, and initialize it
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] childName
///        PCSTR   - Child thread name
/// \~english @param [in] CbInitialize
///        CbFuncPtr   - Pointer to the callback function for initializing child thread
/// \~english @param [in] CbShutdown
///        CbFuncPtr   - Pointer to the callback function for stopping child thread
/// \~english @retval HANDLE handle for communicate with child thread
/// \~english @retval NULL  Failure to get HANDLE
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize, CbShutdown) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
/// \~english @par Detail
///        This API creates child thread and returns handle for communicate with child thread.\n
///        Generated child thread creates dispatcher for itself, and initializes the dispatcher. \n
///        It starts to mainloop that receives request or notification,
///        and runs registered callback to dispatcher for child thread.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThread(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize, CbFuncPtr CbShutdown);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateChildThreadWithPriority
/// \~english @par Brief
///        Create child thread with specified priority and dispatcher for child thread, and initialize it
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] childName
///        PCSTR   - Child thread name
/// \~english @param [in] CbInitialize
///        CbFuncPtr   - Pointer to the callback function for initializing child thread
/// \~english @param [in] CbShutdown
///        CbFuncPtr   - Pointer to the callback function for stopping child thread
/// \~english @param [in] schedPrio
///        SI_32   - priority of child thread
///                  (If INHERIT_PARENT_THREAD_PRIO specified, inherit from parent thread priority)
/// \~english @retval HANDLE handle for communicate with child thread
/// \~english @retval NULL  Failure to get HANDLE
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize,CbShutdown) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
/// \~english @par Detail
///        This API creates child thread with specified priority(schedPrio) and
///        returns handle for communicate with child thread.\n
///        The thread scheduling policy of generated child thread is fixed to first in-first out scheduling
///        (eFrameworkunifiedSchedPolicyFIFO).\n
///        Generated child thread creates dispatcher for itself, and initializes the dispatcher. \n
///        It starts to mainloop that receives request or notification,
///        and runs registered callback to dispatcher for child thread.\n
///        \n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThreadWithPriority(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize
                                        , CbFuncPtr CbShutdown, SI_32 schedPrio);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateDispatcherChild
/// \~english @par Breif
///        Creates and initializes a handle for the child thread's application
/// \~english @param [out] hChildApp
///        HANDLE &  -  A handle for the child thread's application
/// \~english @param [in] childName
///        PCSTR   - Child thread name
/// \~english @param [in] parentName
///        PCSTR   - Parent thread name
/// \~english @retval eFrameworkunifiedStatusOK succss
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusNullPointer Framework generation failure
/// \~english @par Prerequisite
///       - A child thread has been created.
/// \~english @par Change of internal state
///       - The internal state is not changed.
/// \~english @par Conditions of processing failure
///       - If the childName to the child thread name of the application specified in the arguments is NULL [eFrameworkunifiedStatusInvldParam]
///       - If the parentName to the parent thread name of the application specified in the arguments is NULL [eFrameworkunifiedStatusInvldParam]
///       - When the parent thread name (parentName) of the application specified in the arguments exceeds 16 bytes [eFrameworkunifiedStatusInvldParam]
///       - When the normalized message queue name from the childName specified in the arguments exceeds 16 bytes [eFrameworkunifiedStatusNullPointer]
///       - Failed to open a message queue (mq_open) [eFrameworkunifiedStatusNullPointer]
///       - When acquisition (malloc) of the message queue management information area fails [eFrameworkunifiedStatusNullPointer]
///       - If the creation (socket, bind, listen) of sockets for error monitoring fails [eFrameworkunifiedStatusFail]
///       - When a message queue is created with the name of an already registered thread [eFrameworkunifiedStatusFail]
///       - Failed to create epoll instances (epoll_create1) [eFrameworkunifiedStatusInvldHandle]
///       - Failed to register Dispatcher message queue descriptor for epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///       - Failed to create file descriptor for receiving events to Dispatcher (eventfd) [eFrameworkunifiedStatusFail]
///       - Failed to register file descriptor for receiving Dispatcher events to epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
///       - Failed to register socket for monitoring Dispatcher error in epoll instances (epoll_ctl) [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///       This API creates a handle for the application of the child thread, initializes it, and returns the handle.
/// \~english @par Classification
///          Public
/// \~english @par Type
///        Open Close
/// \~english @see
///
/// \~english @par Brief
/// Create and initialize an threads dispatcher.
///
/// \~english \param [out] hChildApp
///         HANDLE& - Reference of application handle
/// \~english \param [in] childName
///         PCSTR - create thread name
/// \~english \param [in] parentName
///         PCSTR - parent thread name
///
/// \~english \return status
///         EFrameworkunifiedStatus - eFrameworkunifiedStatusOK if Success
///         possible errors from call CreateDispatcher
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateDispatcherChild(HANDLE &hChildApp, PCSTR childName, PCSTR parentName);  // NOLINT  (readability/nolint)

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateChildThreadWithAttribute
/// \~english @par Brief
///        Create child thread with specified thread attributes and dispatcher for child thread, and initialize it
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] childName
///        PCSTR   - Child thread name
/// \~english @param [in] CbInitialize
///        CbFuncPtr   - Pointer to the callback function for initializing child thread
/// \~english @param [in] CbShutdown
///        CbFuncPtr   - Pointer to the callback function for stopping child thread
/// \~english @param [out] attr
///        FrameworkunifiedChildThreadAttr*   - Pointer to thread attributes
/// \~english @par
///        FrameworkunifiedChildThreadAttr Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedChildThreadAttr
///                 {
///                   EFrameworkunifiedSchedPolicy schedPolicy;  // Thread scheduling policy(Default is eFrameworkunifiedSchedPolicyInherit)
///                   SI_32 schedPriority;          // Thread priority(Default is INHERIT_PARENT_THREAD_PRIO)
///                 } FrameworkunifiedChildThreadAttr;
///            @endcode
/// \~english @par
///        enum EFrameworkunifiedSchedPolicy Variable
/// \~english @code
///                 typedef enum _EFrameworkunifiedSchedPolicy {
///                   eFrameworkunifiedSchedPolicyInherit = 0,  // Inherit from parent thread.
///                   eFrameworkunifiedSchedPolicyTSS,          // Time Sharing System scheduling
///                   eFrameworkunifiedSchedPolicyFIFO,         // First in-first out scheduling
///                   eFrameworkunifiedSchedPolicyRR,           // Round-robin scheduling
///                   eFrameworkunifiedSchedPolicyMAX           // EFrameworkunifiedSchedPolicy Max(Not to be used.)
///                 } EFrameworkunifiedSchedPolicy;
///            @endcode
/// \~english @retval HANDLE handle for communicate with child thread
/// \~english @retval NULL  Failure to get HANDLE
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [NULL]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [NULL]
///        - Message queue name specified in the argument (childName) is not appropriate (NULL, over 16byte). [NULL]
///        - Callback function pointer specified in the argument (CbInitialize,CbShutdown) is NULL. [NULL]
///        - Thread attribute specified in the argument (attr) is NULL. [NULL]
///        - Failed to set the inherit-scheduler attribute (pthread_attr_setinheritsched) of the child thread. [NULL]
///        - Failed to initialize barrier object (pthread_barrier_init) for thread synchronization. [NULL]
///        - Failed to create child thread (pthread_create). [NULL]
///        - Failed to wait thread synchronization (pthread_barrier_wait). [NULL]
///        - Can not be acquired memory for message queue name(malloc). [NULL]
///        - Failed to open message queue(mq_open). [NULL]
/// \~english @par Detail
///        This API creates child thread with specified thread attributes(attr)
///        and returns handle for communicate with child thread.\n
///        Generated child thread creates dispatcher for itself, and initializes the dispatcher. \n
///        It starts to mainloop that receives request or notification,
///        and runs registered callback to dispatcher for child thread.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
HANDLE FrameworkunifiedCreateChildThreadWithAttribute(HANDLE hApp, PCSTR childName, CbFuncPtr CbInitialize
                                         , CbFuncPtr CbShutdown, const FrameworkunifiedChildThreadAttr *attr);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateChildThreadAttrInit
/// \~english @par Brief
///        Initialize thread attribute object.
/// \~english @param [out] attr
///        FrameworkunifiedChildThreadAttr*   - Pointer to thread attribute
/// \~english @par
///        FrameworkunifiedChildThreadAttr Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedChildThreadAttr
///                 {
///                   EFrameworkunifiedSchedPolicy schedPolicy;  // Thread scheduling policy(Default is eFrameworkunifiedSchedPolicyInherit)
///                   SI_32 schedPriority;          // Thread priority(Default is INHERIT_PARENT_THREAD_PRIO)
///                 } FrameworkunifiedChildThreadAttr;
///            @endcode
/// \~english @par
///        enum EFrameworkunifiedSchedPolicy Variable
/// \~english @code
///                 typedef enum _EFrameworkunifiedSchedPolicy {
///                   eFrameworkunifiedSchedPolicyInherit = 0,  // Inherit from parent thread.
///                   eFrameworkunifiedSchedPolicyTSS,          // Time Sharing System scheduling
///                   eFrameworkunifiedSchedPolicyFIFO,         // First in-first out scheduling
///                   eFrameworkunifiedSchedPolicyRR,           // Round-robin scheduling
///                   eFrameworkunifiedSchedPolicyMAX           // EFrameworkunifiedSchedPolicy Max(Not to be used.)
///                 } EFrameworkunifiedSchedPolicy;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @par Prerequisite
///        None
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - the argument attr is NULL. [eFrameworkunifiedStatusNullPointer]
/// \~english @par Detail
///        This API initialize thread attribute (attr) by default value of NS framework.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedCreateChildThreadWithAttribute
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateChildThreadAttrInit(FrameworkunifiedChildThreadAttr *attr);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedCreateChildThreadAttrSetSched
/// \~english @par Brief
///        Initialize thread attribute object with specified value.
/// \~english @param [out] attr
///        FrameworkunifiedChildThreadAttr*   - Pointer to thread attribute
/// \~english @param [in] policy
///        EFrameworkunifiedSchedPolicy   - Thread shcheduling policy at thread start
/// \~english @param [in] priority
///        SI_32   - Thread priority at thread start
/// \~english @par
///        FrameworkunifiedChildThreadAttr Structure
/// \~english @code
///                 typedef struct _FrameworkunifiedChildThreadAttr
///                 {
///                   EFrameworkunifiedSchedPolicy schedPolicy;  // Thread scheduling policy(Default is eFrameworkunifiedSchedPolicyInherit)
///                   SI_32 schedPriority;          // Thread priority(Default is INHERIT_PARENT_THREAD_PRIO)
///                 } FrameworkunifiedChildThreadAttr;
///            @endcode
/// \~english @par
///        enum EFrameworkunifiedSchedPolicy Variable
/// \~english @code
///                 typedef enum _EFrameworkunifiedSchedPolicy {
///                   eFrameworkunifiedSchedPolicyInherit = 0,  // Inherit from parent thread.
///                   eFrameworkunifiedSchedPolicyTSS,          // Time Sharing System scheduling
///                   eFrameworkunifiedSchedPolicyFIFO,         // First in-first out scheduling
///                   eFrameworkunifiedSchedPolicyRR,           // Round-robin scheduling
///                   eFrameworkunifiedSchedPolicyMAX           // EFrameworkunifiedSchedPolicy Max(Not to be used.)
///                 } EFrameworkunifiedSchedPolicy;
///            @endcode
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusNullPointer NULL pointer specified
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @par Prerequisite
///       None
/// \~english @par Change of internal state
///       - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///       - the argument attr is NULL. [eFrameworkunifiedStatusNullPointer]
///       - the argument policy is out of range
///         (less than eFrameworkunifiedSchedPolicyInherit, over eFrameworkunifiedSchedPolicyMAX). [eFrameworkunifiedStatusInvldParam]
/// \~english @par Detail
///        This API initializes thread attribute object for child thread with specified value.\n
///        \n
/// \~english @par Classification
///        Public
/// \~english @see
///        FrameworkunifiedCreateChildThreadWithAttribute
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedCreateChildThreadAttrSetSched(FrameworkunifiedChildThreadAttr *attr, EFrameworkunifiedSchedPolicy policy, SI_32 priority);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedStartChildThread
/// \~english @par Brief
///        Send initialize request to the child thread.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] hChildQ
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @param [in] length
///        UI_32   - Length of send message data
/// \~english @param [in] data
///        PCVOID   - Pointer to message data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state is depend on application implements.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Data buffer specified in the argument (data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///        - Thread name specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldQName]
///        - Message queue name of child thread specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to access to shared memory for sending message. [eFrameworkunifiedStatusErrOther]
///        - Type of message queue to child thread is not for sending. [eFrameworkunifiedStatusInvldHndlType]
///        - Message queue for sending is full. [eFrameworkunifiedStatusMsgQFull]
///        - File descriptor of sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during message sending. [eFrameworkunifiedStatusErrNoEINTR]
///        - Any errors occur during the transmission of message to the child thread. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This API sends initialize request to the child thread from parent thread.\n
///        Taking this request,
///        child thread runs callback function for initialize that specified when the thread create.
/// \~english @par
///        \n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        Open Close
/// \~english @see
///        FrameworkunifiedStopChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedStartChildThread(HANDLE hApp, HANDLE hChildQ, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedStopChildThread
/// \~english @par Brief
///        Send shutdown request to the child thread.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] hChildQ
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @param [in] length
///        UI_32   - Length of send message data
/// \~english @param [in] data
///        PCVOID   - Pointer to message data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state is depend on application implements.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Data buffer specified in the argument (data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///        - Thread name specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldQName]
///        - Message queue name of child thread specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to access to shared memory for sending message. [eFrameworkunifiedStatusErrOther]
///        - Type of message queue to child thread is not for sending. [eFrameworkunifiedStatusInvldHndlType]
///        - Message queue for sending is full. [eFrameworkunifiedStatusMsgQFull]
///        - File descriptor of sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during message sending. [eFrameworkunifiedStatusErrNoEINTR]
///        - Any errors occur during the transmission of message to the child thread. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This API sends shutdown request to the child thread.\n
///        Taking this request,
///        child thread runs  callback function for shutdown that specified when the thread create.\n
///        This API does not terminate child thread.
///        Use FrameworkunifiedDestroyChildThread to terminate child thread from parent thread.
/// \~english @par
///        \n
/// \~english @par Classification
///        Public
/// \~english @see
///        FrameworkunifiedStartChildThread FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedStopChildThread(HANDLE hApp, HANDLE hChildQ, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedDestroyChildThread
/// \~english @par Brief
///        Terminate child thread.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] hChildQ
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @retval eFrameworkunifiedStatusThreadNotExist Thread is not exist
/// \~english @retval eFrameworkunifiedStatusThreadSelfJoin Self thread specified
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread
///          (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state is depend on application implements.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldParam]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldParam]
///        - HANDLE specified in the argument (hChildQ) is NULL. [eFrameworkunifiedStatusInvldParam]
///        - HANDLE specified in the argument (hChildQ) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Thread name specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldQName]
///        - Message queue name of child thread specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
///        - Type of message queue to child thread is not for sending. [eFrameworkunifiedStatusInvldHndlType]
///        - Message queue for sending is full. [eFrameworkunifiedStatusMsgQFull]
///        - File descriptor of sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during message sending. [eFrameworkunifiedStatusErrNoEINTR]
///        - Message data size is invalid. [eFrameworkunifiedStatusInvldBufSize]
///        - Any errors occur during the transmission of message to the child thread. [eFrameworkunifiedStatusFail]
///        - Child thread is invalid (thread exit or invalid) [eFrameworkunifiedStatusInvldHandle]
///        - Another thread is already waiting to join with this child thread. [eFrameworkunifiedStatusInvldParam]
///        - Already child thread is in termination process [eFrameworkunifiedStatusInvldParam]
///        - Thread specified in argument is self thread. [eFrameworkunifiedStatusThreadSelfJoin]
///        - Failed to close message queue specified in the argument (hChildQ). [eFrameworkunifiedStatusInvldHandle]
/// \~english @par Detail
///       This API sends terminate request to child thread , and waits to terminate child thread.\n
///       Taking this request, child thread  terminates itself.\n
///       After child thread terminates, parent thread destroys handle for communicate with child thread.
/// \~english @par Classification
///          Public
/// \~english @see FrameworkunifiedCreateChildThread FrameworkunifiedCreateChildThreadWithAttribute FrameworkunifiedCreateChildThreadWithPriority
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedDestroyChildThread(HANDLE hApp, HANDLE hChildQ);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSendChild
/// \~english @par Brief
///        Send a message to the child thread.
/// \~english @param [in] hApp
///        HANDLE   - Handle for Application of parent thread
/// \~english @param [in] hChildQ
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @param [in] iCmd
///        UI_32   - Command of message
/// \~english @param [in] length
///        UI_32   - Length of send message data
/// \~english @param [in] data
///        PCVOID   - Pointer to message data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildQ) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Data buffer specified in the argument (data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///        - Thread name specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldQName]
///        - Message queue name of child thread specified in the argument (hChildQ) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to access to shared memory for sending message. [eFrameworkunifiedStatusErrOther]
///        - Type of message queue to child thread is not for sending. [eFrameworkunifiedStatusInvldHndlType]
///        - Message queue for sending is full. [eFrameworkunifiedStatusMsgQFull]
///        - File descriptor of sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during message sending. [eFrameworkunifiedStatusErrNoEINTR]
///        - Any errors occur during the transmission of message to the child thread. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This API sends a message to the child thread from parent thread.\n
///        This API is available only from parent thread.
/// \~english @par
///        \n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedSendParent
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendChild(HANDLE hApp, HANDLE hChildQ, UI_32 iCmd, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedSendParent
/// \~english @par Brief
///        Send a message to the parent thread.
/// \~english @param [in] hChildApp
///        HANDLE   - Handle for Application of child thread(get as argument of callback function)
/// \~english @param [in] iCmd
///        UI_32   - Command of message
/// \~english @param [in] length
///        UI_32   - Length of send message data
/// \~english @param [in] data
///        PCVOID   - Pointer to message data
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusInvldBuf Invalid buffer
/// \~english @retval eFrameworkunifiedStatusInvldQName Illegal Message Queue name
/// \~english @retval eFrameworkunifiedStatusErrOther Other error has occurred(Cannot access shared memory, etc.)
/// \~english @retval eFrameworkunifiedStatusInvldHndlType Invalid type of handle
/// \~english @retval eFrameworkunifiedStatusMsgQFull Message queue is full
/// \~english @retval eFrameworkunifiedStatusErrNoEBADF Invalid File-Descriptor
/// \~english @retval eFrameworkunifiedStatusErrNoEINTR An interrupt is generated by the system call (signal)
/// \~english @retval eFrameworkunifiedStatusInvldBufSize Invalid buffer-size
/// \~english @retval eFrameworkunifiedStatusFail Some sort of error occurred
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hChildApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildApp) is not appropriate (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Message queue for parent thread in HANDLE (hChildApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - Message queue for parent thread in HANDLE (hChildApp) is not appropriate
///          (which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Data buffer specified in the argument (data) is NULL. [eFrameworkunifiedStatusInvldBuf]
///        - Thread name specified in the argument (hChildApp) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldQName]
///        - The name of message queue for parent thread specified in the argument (hChildApp) is not appropriate
///          (The name is NULL, more than 20byte). [eFrameworkunifiedStatusInvldHandle]
///        - Failed to access to shared memory for sending message. [eFrameworkunifiedStatusErrOther]
///        - Type of message queue to parent thread is not for sending. [eFrameworkunifiedStatusInvldHndlType]
///        - Message queue for sending is full. [eFrameworkunifiedStatusMsgQFull]
///        - File descriptor of sending message is invalid. [eFrameworkunifiedStatusErrNoEBADF]
///        - Interruption by the system call (signal) has occurred during message sending. [eFrameworkunifiedStatusErrNoEINTR]
///        - Any errors occur during the transmission of message to the parent thread. [eFrameworkunifiedStatusFail]
/// \~english @par Detail
///        This API sends a message to the parent thread from child thread.\n
///        This API is available only from child thread.
/// \~english @par
///        \n
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedSendChild
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedSendParent(HANDLE hChildApp, UI_32 iCmd, UI_32 length, PCVOID data);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedJoinChild
/// \~english @par Brief
///        Wait for child thread terminates.
/// \~english @param [in] hChildApp
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusThreadNotExist Thread is not exist
/// \~english @retval eFrameworkunifiedStatusInvldParam Invalid parameter
/// \~english @retval eFrameworkunifiedStatusThreadSelfJoin Self thread specified
/// \~english @retval eFrameworkunifiedStatusFail thread exit or invalid
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hChildApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - Child thread is invalid (thread exit or invalid) [eFrameworkunifiedStatusFail]
///        - Another thread is already waiting to join with this child thread. [eFrameworkunifiedStatusInvldParam]
///        - Already child thread is in termination process [eFrameworkunifiedStatusInvldParam]
///        - Thread specified in argument is self thread. [eFrameworkunifiedStatusThreadSelfJoin]
/// \~english @par Detail
///        It waits for terminate of child thread it was created by FrameworkunifiedCreateChildThread
///        (or similar one) in the parent thread.\n
///        This API blocks the calling thread until child thread terminates.\n
///        This API does not terminate child thread.
///        Use FrameworkunifiedDestroyChildThread to terminate child thread from parent thread.
/// \~english @par Classification
///        Public
/// \~english @see
///        FrameworkunifiedCreateChildThread, FrameworkunifiedCreateChildThreadWithPriority, FrameworkunifiedDestroyChildThread
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedJoinChild(HANDLE hChildApp);

////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup FrameworkunifiedGetChildThreadPriority
/// \~english @par Brief
///        Get the child thread priority.
/// \~english @param [in] hChildApp
///        HANDLE   - Handle for communicate with child thread(returned by FrameworkunifiedCreateChildThread)
/// \~english @param [out] threadPrio
///        PSI_32   - Dispatcher file descriptor
/// \~english @retval eFrameworkunifiedStatusOK Success
/// \~english @retval eFrameworkunifiedStatusInvldHandle Invalid handle
/// \~english @retval eFrameworkunifiedStatusThreadNotExist Thread is not exist
/// \~english @retval eFrameworkunifiedStatusFault Error occured during function
/// \~english @par Prerequisite
///        - Generation/Initialization of Dispatcher for the Application
///          (FrameworkunifiedCreateDispatcherWithoutLoop, etc.) has been done.
///        - Generation/Initialization of Dispatcher for the child thread (FrameworkunifiedCreateChildThread, etc.) has been done.
/// \~english @par Change of internal state
///        - Change of internal state according to the API does not occur.
/// \~english @par Conditions of processing failure
///        - HANDLE specified in the argument (hChildApp) is NULL. [eFrameworkunifiedStatusInvldHandle]
///        - HANDLE specified in the argument (hChildApp) is not appropriate(which is invalid). [eFrameworkunifiedStatusInvldHandle]
///        - child thread priority in the argument (threadPrio) is NULL. [eFrameworkunifiedStatusInvldParam]
///        - Failed to get child thread priority (pthread_getschedparam). [eFrameworkunifiedStatusFault]
///        - If child threads are inalid or already terminated. [eFrameworkunifiedStatusThreadNotExist]
/// \~english @par Detail
///        This API set the child thread priority generated by FrameworkunifiedCreateChildThread to argument threadPrio.
///        Use to get child thread priority from parent thread.
/// \~english @par Classification
///        Public
/// \~english @par Type
///        No match
/// \~english @see
///        FrameworkunifiedCreateChildThread, FrameworkunifiedCreateChildThreadWithPriority
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus FrameworkunifiedGetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio);
#endif  // __NSFRAMEWORK_NFRAMEWORKCORE_MULTITHREADING__  NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
