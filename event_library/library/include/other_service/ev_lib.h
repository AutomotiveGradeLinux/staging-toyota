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

/**
 * @file ev_lib.h
 * @brief  \~english Event library -- API define head file
 */

#ifndef OTHERSERVICE_EV_LIB_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_EV_LIB_H_  // NOLINT(build/header_guard)

#include <agl_types_obsoluted.h>
#include <agldd/ev_common.h>
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup other_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup event_library
 *  @ingroup other_service
 *  @{
 */

#ifndef __KERNEL__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Create/Delete flag and message queue */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag
/// \~english @par Brief
///       Create flag for event flag by the flag_id.
/// \~english @param [in] flag_id
///       EV_ID    - Flag ID of the flag event
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist Duplication Error
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id: The ID is not created by function of EV_moduleID_to_flagID().
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - flag_id registered in launch thread
///       - Flag corresponding to flag_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Creates flag with ID specified by the argument.
///       An error occurs if flag of the specified ID already exists.
/// \~english @see  evk_open, evk_close, evk_ioctl, evk_create_flag, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag(EV_ID flag_id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag64
/// \~english @par Brief
///       Create flag for event flag by the flag_id.
/// \~english @param [in] flag_id
///       EV_ID    - Flag ID of the flag event
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist Duplication Error
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id: The ID is not created by function of EV_moduleID_to_flag64ID ().
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - flag_id registered in launch thread
///       - Flag corresponding to flag_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Creates flag with ID specified by the argument.
///       An error occurs if flag of the specified ID already exists.
/// \~english @see  evk_open, evk_close, evk_ioctl, evk_create_flag, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag64(EV_ID flag_id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_queue
/// \~english @par Brief
///       Create message queue with the queue_id.
/// \~english @param [in] queue_id
///       EV_ID  - queue_id of message event
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [in] length
///       UINT8  - length of message queue (Maximum accumulated number of the messages)
/// \~english @param [in] max_bytes
///       UINT16  - the maximum number of bytes per 1 message(MAX 2048)
/// \~english @param [in] type
///       EV_Message_Queue_Type  - classification of the processing when the queue received more event
///                              - in the queue full state.
/// \~english @par
///       EV_Message_Queue_Type
/// \~english @code
///       typedef UINT8 EV_Message_Queue_Type;
///          -  EV_MESSAGE_QUEUE_TYPE_BUSY = 0  // Return error(EV_ERR_Busy) to origin of message transmission
///          -  EV_MESSAGE_QUEUE_TYPE_FIFO = 1  // Delete the top message of the queue, and shift message
///                                             // (Delete the oldest message)
///          -  EV_MESSAGE_QUEUE_TYPE_REPLACE = 2  // Overwrite the last message of the queue
///                                                // (Overwrite the newest message)
///        @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist Duplication Error
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id is not created by function of EV_moduleID_to_queueID ().
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - queue_id registered in launch thread
///       - Message queue corresponding to queue_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Message queue pool is NULL
///       - Message queue generation corresponding to queue_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Creates message queue with ID specified by the argument.
///       When queue is created, a memory area corresponding to "length" x "max_bytes" is dynamically secured.
///       An error occurs if queue of the specified ID already exists.
/// \~english @see evk_open, evk_close, evk_ioctl, evk_create_message_queue, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_queue(EV_ID                queue_id,   // NOLINT (readability/nolint)
                      UINT8                 length,     // NOLINT (readability/nolint)
                      UINT16                max_bytes,  // NOLINT (readability/nolint)
                      EV_Message_Queue_Type type);      // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag_auto_id
/// \~english @par Brief
///       Automatically assign flag ID, and creates flag with flag ID.
/// \~english @param [out] flag_id
///       EV_ID*    - flag_id assigned automatically
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Automatically assign flag ID, and creates flag with flag ID.
///       It is the same as "EV_create_flag" except that flag ID is automatically allocated.
/// \~english @see EV_create_flag, evk_open, evk_close, evk_ioctl, evk_create_flag, evk_alloc_flag_id, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag_auto_id(/* OUT */EV_ID *flag_id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag64_auto_id
/// \~english @par Brief
///       Automatically assign flag ID, and creates flag with flag ID.
/// \~english @param [out] flag_id
///       EV_ID*    - flag_id assigned automatically
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Automatically assign flag ID, and creates flag with flag ID.
///       It is the same as "EV_create_flag" except that flag ID is automatically allocated.
/// \~english @see EV_create_flag, evk_open, evk_close, evk_ioctl, evk_create_flag, evk_alloc_flag_id, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag64_auto_id(/* OUT */EV_ID *flag_id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_queue_auto_id
/// \~english @par Brief
///       Automatically assign queue ID, and creates message queue with queue ID.
///                                         It stores queue_id assigned automatically in argument.
/// \~english @param [out] queue_id
///       EV_ID*  - queue_id assigned automatically
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [in] length
///       UINT8  - length of message queue (Maximum accumulated number of the messages)
/// \~english @param [in] max_bytes
///       UINT16  - the maximum number of bytes per 1 message(MAX 2048)
/// \~english @param [in] type
///       EV_Message_Queue_Type  - classification of the processing when the queue received more event
///                                in the queue full state.
/// \~english @par
///       EV_Message_Queue_Type
/// \~english @code
///       typedef UINT8 EV_Message_Queue_Type;
///          -  EV_MESSAGE_QUEUE_TYPE_BUSY = 0  // Return error(EV_ERR_Busy) to origin of message transmission
///          -  EV_MESSAGE_QUEUE_TYPE_FIFO = 1  // Delete the top message of the queue, and shift message
///                                             // (Delete the oldest message)
///          -  EV_MESSAGE_QUEUE_TYPE_REPLACE = 2  // Overwrite the last message of the queue
///                                                // (Overwrite the newest message)
///        @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Thread is unregistered and threads started up to the maximum of event thread(16 threads)
///          - EV_ERR_Thread_Over
///       - Message queue corresponding to queue_id already exists
///       - Securing free space of thread information management table (malloc) Failure
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Message queue pool is NULL
///       - Message queue generation corresponding to queue_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Automatically assign queue id, and creates message queue with queue id, It stores queue_id
///       assigned automatically in argument.
///       It is the same as "EV_queue_flag" except that queue ID is automatically allocated.
/// \~english @see EV_create_queue, evk_open, evk_close, evk_ioctl, evk_create_message_queue,
///                evk_alloc_queueID, evk_set_poll
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_queue_auto_id(/* OUT */EV_ID       *queue_id,  // NOLINT (readability/nolint)
                              UINT8                 length,     // NOLINT (readability/nolint)
                              UINT16                max_bytes,  // NOLINT (readability/nolint)
                              EV_Message_Queue_Type type);      // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_destroy_flag
/// \~english @par Brief
///       Deletes flag with ID specified by the argument.
/// \~english @param [in] flag_id
///       EV_ID    - Flag ID of the flag event
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Flag corresponding to queue_id doesn't exist
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Deletes flag with ID specified by the argument.
///       The user can only delete flag created by user process.
///       Due to deletion, program waiting for this flag (poll or wait) wakes up.
/// \~english @see evk_close, evk_ioctl, evk_destroy_queue
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_destroy_flag(EV_ID queue_id);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_destroy_queue
/// \~english @par Brief
///       Delete queue for message event by the queue_id.
/// \~english @param [in] queue_id
///       EV_ID    - queue_id of the message event
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Message queue corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Deletes message queue with ID specified by the argument.
///       The user can only delete message queue created by user process.
///       Due to deletion, program waiting for this message queue (poll or wait) wakes up.
/// \~english @see EV_destroy_flag, evk_close, evk_ioctl, evk_destroy_queue
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_destroy_queue(EV_ID queue_id);  // NOLINT (readability/nolint)

/* Send event */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_set_flag
/// \~english @par Brief
///       Set Event flag
/// \~english @param [in] flag_id
///       EV_ID    - Flag ID of the event flag
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [in] bits
///       UINT32      - event flag bit pattern
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The flag_id of event which has not been created.
///       - Flag corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sets bit pattern specified by argument to flag of ID specified by argument.
///       When setting a plurality of bit patterns in the same flag, "only OR
///       operation result of all bit patterns" is held in flag.
/// \~english @see evk_open, evk_close, evk_ioctl, evk_store_flag
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_set_flag(EV_ID flag_id, UINT32 bits);  // NOLINT (readability/nolint)

/* Send event */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_set_flag64
/// \~english @par Brief
///       Set Event flag
/// \~english @param [in] flag_id
///       EV_ID    - Flag ID of the event flag
/// \~english @par
///         EV_ID type parameter
///   @code

///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [in] bits
///       UINT32      - event flag bit pattern
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The flag_id of event which has not been created.
///       - Flag corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sets bit pattern specified by argument to flag of ID specified by argument.
///       When setting a plurality of bit patterns in the same flag, "only OR operation
///       result of all bit patterns" is held in flag.
/// \~english @see evk_open, evk_close, evk_ioctl, evk_store_flag
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_set_flag64(EV_ID flag_id, UINT64 bits);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_send_message
/// \~english @par Brief
///       Send Message event
/// \~english @param [in] queue_id
///       EV_ID    - Queue ID of the message destination
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [in] bytes
///       UINT16      - The bytes of the send message
/// \~english @param [in] message
///       const void *  - Pointer to a transmitting byte line
/// \~english @param [in] senderInfo
///       UINT32     - Message Source(It is specified when it is used for application identification.
///                    When do not use it, appoint 0.)
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Busy Queue overflow
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Message queue corresponding to queue_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Queue overflow in kernel space
///          - EV_ERR_Busy
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Invalid queue types
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sends message specified by argument to message queue of ID specified by argument.
///       An error occurs when message queue of the specified ID doesn't exist or
///       message size exceeds the specified size.
///       When message queue is FULL, the processing differs depending on the type of message queue specified
///       at the time of creation message queue.
/// \~english @see evk_open, evk_close, evk_ioctl, evk_store_message
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_send_message(EV_ID      queue_id,     // NOLINT (readability/nolint)
                      UINT16      bytes,        // NOLINT (readability/nolint)
                      const void  *message,     // NOLINT (readability/nolint)
                      UINT32      senderInfo);  // NOLINT (readability/nolint)

/* \~english Get events in the order of arrivals */
////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_next_event
/* \~english get events by reaching time */
/// \~english @par Brief  get the first reached event(non-block)
/// \~english @param [out] ev
///       event Structure
/// \~english @par
///         EV_Event struct
///   @code
///             typedef unsigned int UINT32;
///             typedef UINT32 EV_Type;
///             typedef UINT32 EV_ID;
///             #define EV_MAX_MESSAGE_LENGTH 2048
///             enum ev_message_queue_type {
///               EV_MESSAGE_QUEUE_TYPE_BUSY,
///               EV_MESSAGE_QUEUE_TYPE_FIFO,
///               EV_MESSAGE_QUEUE_TYPE_REPLACE,
///             };
///
///             typedef struct {
///               EV_ID flagID;
///               UINT32 bits;
///             } EV_Flag;
///
///             typedef struct {
///               EV_ID flagID;
///               UINT64 bits;
///             } EV_Flag64;
///
///             typedef struct {
///              EV_ID queueID;
///               UINT32 senderInfo;
///               UINT32 length;
///               UINT32 dummy;
///               UINT8  message[EV_MAX_MESSAGE_LENGTH];
///             } EV_Message;
///
///             typedef struct {
///               EV_Type type;  // please reference  ev_message_queue_type.
///               union {
///                 EV_Flag flag;
///                 EV_Flag64 flag64;
///                 EV_Message message;
///               } u;
///             } EV_Event;
///
///         @endcode

/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_Thread: unregistered thread
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - unregistered thread
///          - EV_ERR_Invalid_Thread
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   Get the first reached event which occurred for
///   flags or message queues created in current process.
///   If there is no event, set ev->type to EV_EVENT_None, then return immediately.
///   The event is flag or message is decided by
///   the ev->type is EV_EVENT_Flag or EV_EVENT_Message.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_next_event(/* OUT */EV_Event *ev);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_peek_next_event
/// \~english @par Brief  get the first reached event(non-destructive)
/// \~english @param [out] ev
///       event Structure
///   @code
///             typedef unsigned int UINT32;
///             typedef UINT32 EV_Type;
///             typedef UINT32 EV_ID;
///             #define EV_MAX_MESSAGE_LENGTH 2048
///             enum ev_message_queue_type {
///               EV_MESSAGE_QUEUE_TYPE_BUSY,
///               EV_MESSAGE_QUEUE_TYPE_FIFO,
///               EV_MESSAGE_QUEUE_TYPE_REPLACE,
///             };
///
///             typedef struct {
///               EV_ID flagID;
///               UINT32 bits;
///             } EV_Flag;
///
///             typedef struct {
///               EV_ID flagID;
///               UINT64 bits;
///             } EV_Flag64;
///
///             typedef struct {
///              EV_ID queueID;
///               UINT32 senderInfo;
///               UINT32 length;
///               UINT32 dummy;
///               UINT8  message[EV_MAX_MESSAGE_LENGTH];
///             } EV_Message;
///
///             typedef struct {
///               EV_Type type;  // please reference  ev_message_queue_type.
///               union {
///                 EV_Flag flag;
///                 EV_Flag64 flag64;
///                 EV_Message message;
///               } u;
///             } EV_Event;
///
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_Thread: unregistered thread
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - unregistered thread
///          - EV_ERR_Invalid_Thread
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   Get the first reached event which occurred for
///   flags or message queues created in current process non-destructively.
///   If the event is not getted from other processes, then call the function of
///   EV_get_next_event() or EV_peek_next_event(), it will return a same event.
///   If there is no event, set ev->type to EV_EVENT_None, then return immediately.
///   The event is flag or message is decided by
///   ev->type is EV_EVENT_Flag or EV_EVENT_Message.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_peek_next_event(/* OUT */EV_Event *ev);  // NOLINT (readability/nolint)

/* Get eventflag (ID specified) */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_flag
/// \~english @par Brief
///       Get event flag(non-block)
/// \~english @param [in] flag_id
///       EV_ID  - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] flag
///       EV_Flag * - event flag structure
/// \~english @par
///       EV_Flag structure
/// \~english @code
///       typedef struct {
///         EV_ID flagID;  // Flag ID
///         UINT32 bits;   // event flag bit pattern
///       } EV_Flag;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets bit pattern specified by argument to flag of ID specified by argument.
///       The user can only get flag created by user process, and clear bit pattern of acquired flag.
///       An error occurs if flag of the specified ID doesn't exist.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_flag(EV_ID flag_id, /* OUT */EV_Flag *flag);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_wait_flag
/// \~english @par Brief
///       Get event flag(block)
/// \~english @param [in] flag_id
///       EV_ID  - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] flag
///       EV_Flag * - event flag structure
/// \~english @par
///       EV_Flag structure
/// \~english @code
///       typedef struct {
///         EV_ID flagID;  // Flag ID
///         UINT32 bits;   // event flag bit pattern
///       } EV_Flag;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Interrupted signal interrupt
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Task blocking was canceled by signal interrupt
///          - EV_ERR_Interrupted
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets bit pattern specified by argument to flag of ID specified by argument.
///       The user can only get flag created by user process, and clear bit pattern of acquired flag.
///       An error occurs if flag of the specified ID doesn't exist.
///       If bit pattern is not set in flag, block until bit pattern is set.
///       However, if a signal interrupt or corresponding flag is deleted, it will abort and return an error.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_wait_flag(EV_ID flag_id, /* OUT */EV_Flag *flag);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_peek_flag
/// \~english @par Brief get flag event(non-destructive)
/// \~english @param  [in] flag_id
///       ID of message event queue
/// \~english @param  [out] flag
///       flag struct
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @par
///         EV_Flag Structure
///   @code
///         typedef struct {
///         EV_ID flagID;
///           UINT32 bits;
///         } EV_Flag;
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: The specified flag ID is not existing,
///   or it is not created in current process.
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Task blocking was canceled by signal interrupt
///          - EV_ERR_Interrupted
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   Get a flag event whose ID is specified non-destructively.
///   If there is no event, just return.
///   If there is no event, set flag->flagID to EV_NO_ID.
///   You can judge that there is no event, even though flag->bits is 0.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_peek_flag(EV_ID flag_id, /* OUT */EV_Flag *flag);  // NOLINT (readability/nolint)

/* Get eventflag (ID specified) */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_flag64
/// \~english @par Brief
///       Get event flag(non-block)
/// \~english @param [in] flag_id
///       EV_ID  - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] flag
///       EV_Flag * - event flag structure
/// \~english @par
///       EV_Flag structure
/// \~english @code
///       typedef unsigned int UINT32;
///       typedef UINT32 EV_ID
///
///       typedef struct {
///         EV_ID flagID;  // Flag ID
///         UINT32 bits;   // event flag bit pattern
///       } EV_Flag;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets bit pattern specified by argument to flag of ID specified by argument.
///       The user can only get flag created by user process, and clear bit pattern of acquired flag.
///       An error occurs if flag of the specified ID doesn't exist.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_flag64(EV_ID flag_id, /* OUT */EV_Flag64 *flag);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_wait_flag64
/// \~english @par Brief
///       Get event flag(block)
/// \~english @param [in] flag_id
///       EV_ID  - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] flag
///       EV_Flag * - event flag structure
/// \~english @par
///       EV_Flag structure
/// \~english @code
///       typedef unsigned int UINT32;
///       typedef UINT32 EV_ID
///
///       typedef struct {
///         EV_ID flagID;  // Flag ID
///         UINT32 bits;   // event flag bit pattern
///       } EV_Flag;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Interrupted signal interrupt
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Task blocking was canceled by signal interrupt
///          - EV_ERR_Interrupted
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets bit pattern specified by argument to flag of ID specified by argument.
///       The user can only get flag created by user process, and clear bit pattern of acquired flag.
///       An error occurs if flag of the specified ID doesn't exist.
///       If bit pattern is not set in flag, block until bit pattern is set.
///       However, if a signal interrupt or corresponding flag is deleted, it will abort and return an error.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_wait_flag64(EV_ID flag_id, /* OUT */EV_Flag64 *flag);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_peek_flag64
/// \~english @par Brief get flag event(non-destructive)
/// \~english @param  [in] flag_id
///       ID of message event queue
/// \~english @param  [out] flag
///       flag struct
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @par
///         EV_Flag Structure
///   @code
///         typedef struct {
///         EV_ID flagID;
///           UINT32 bits;
///         } EV_Flag;
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: The specified flag ID is not existing,
///   or it is not created in current process.
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Flag corresponding to flag_id doesn't exist
///       --incorrect flag_id:The flag_id of event which has not been created.
///          - EV_ERR_Invalid_ID
///       - Task blocking was canceled by signal interrupt
///          - EV_ERR_Interrupted
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   Get a flag event whose ID is specified non-destructively.
///   If there is no event, just return.
///   If there is no event, set flag->flagID to EV_NO_ID.
///   You can judge that there is no event, even though flag->bits is 0.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_peek_flag64(EV_ID flag_id, /* OUT */EV_Flag64 *flag);  // NOLINT (readability/nolint)

/* Get Message Event (ID specified) */
/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_message
/// \~english @par Brief
///       Get message event(non-block)
/// \~english @param [in] queue_id
///       EV_ID  - Message queue ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] message
///       EV_Message * - Message event structure
/// \~english @par
///       EV_Message structure
/// \~english @code
///       typedef unsigned int UINT32;
///       typedef UINT32 EV_ID
///
///       #define EV_MAX_MESSAGE_LENGTH 2048
///       typedef struct {
///         EV_ID queueID;  // ID of message queue
///         UINT32 senderInfo;  // Message Source
///         UINT32 length;  // length of message
///         UINT32 dummy;  // dummy for padding
///         UINT8  message[EV_MAX_MESSAGE_LENGTH];  // message
///       } EV_Message;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Message queue corresponding to queue_id doesn't exist
///       --incorrect queue_id:The queue_id of essage which has not been created.
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets 1 message specified by argument to message queue of ID specified by argument.
///       The user can only get message queue created by user process, and clear 1 message of acquired message queue.
///       An error occurs if message queue of the specified ID doesn't exist.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_message(EV_ID queue_id, /* OUT */EV_Message *message);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_wait_message
/// \~english @par Brief
///       Get message event(block)
/// \~english @param [in] queue_id
///       EV_ID  - Message queue ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] message
///       EV_Message * - Message event structure
/// \~english @par
///       EV_Message structure
/// \~english @code
///       typedef unsigned int UINT32;
///       typedef UINT32 EV_ID
///
///       #define EV_MAX_MESSAGE_LENGTH 2048
///       typedef struct {
///         EV_ID queueID;  // ID of message queue
///         UINT32 senderInfo;  // Message Source
///         UINT32 length;  // length of message
///         UINT32 dummy;  // dummy for padding
///         UINT8  message[EV_MAX_MESSAGE_LENGTH];  // message
///       } EV_Message;
///       @endcode
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Interrupted signal interrupt
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Message queue corresponding to queue_id doesn't exist
///       --incorrect queue_id:The queue_id of essage which has not been created.
///          - EV_ERR_Invalid_ID
///       - Task blocking was canceled by signal interrupt
///          - EV_ERR_Interrupted
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets 1 message specified by argument to message queue of ID specified by argument.
///       The user can only get message queue created by user process, and clear 1 message of acquired message queue.
///       An error occurs if message queue of the specified ID doesn't exist.
///       If message is not set in message queue, block until message is sent.
///       However, if a signal interrupt or corresponding message queue is deleted, it will abort and return an error.
/// \~english @see
///       evk_ioctl, evk_get_event
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_wait_message(EV_ID queue_id, /* OUT */EV_Message *message);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_peek_message
/// \~english @par Brief  get message event(non-destructive)
/// \~english @param  [in] queue_id
///       ID of message event queue
/// \~english @param  [out] message
///       message event queue struct
/// \~english @par
///         EV_ID define
///   @code
///         typedef UINT32 EV_ID
///         @endcode

/// \~english @par
///         EV_Message struct
///   @code
///         #define EV_MAX_MESSAGE_LENGTH 2048
///         typedef struct {
///            EV_ID queueID;
///            UINT32 senderInfo;
///            UINT32 length;
///            UINT32 dummy;
///            UINT8  message[EV_MAX_MESSAGE_LENGTH];
///          } EV_Message;
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: The specified queue ID is not existing,
///   or it is not created in current process.
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Message queue corresponding to queue_id doesn't exist
///       --incorrect queue_id:The queue_id of essage which has not been created.
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   Get a message event queue whose ID is specified non-destructively.
///   If there is no event, just return.
///   If there is no event, set message->queueID to EV_NO_ID.
///   It is the same with EV_get_message(), except the message is remained in the queue.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_peek_message(EV_ID queue_id, /* OUT */EV_Message *message);  // NOLINT (readability/nolint)

/* \~english Get events(with search criteria) */

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_find_message_by_sender
/* \~english get event(with search sender infomation condition) */
/// \~english @param  [in] queue_id
///       ID of message event queue
/// \~english @param  [in] senderInfo
///       sender information
/// \~english @param  [out] message
///       message event struct
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @par
///         EV_Message struct
///   @code
///         #define EV_MAX_MESSAGE_LENGTH 2048
///         typedef struct {
///            EV_ID queueID;
///            UINT32 senderInfo;
///            UINT32 length;
///            UINT32 dummy;
///            UINT8  message[EV_MAX_MESSAGE_LENGTH];
///          } EV_Message;
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: The specified queue ID is not existing,
///   or it is not created in current process.
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Not find same sender information's message according to input sender information information
///          - EV_ERR_Invalid_ID
///       - Securing free space of thread information management table (malloc) Failure
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Message queue pool is NULL
///       - Message queue generation corresponding to queue_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   If there is a event whose sender information is same with
///   any event in message event queue whose ID is specified, get the event and delete it from queue.
///   If such a message is not existing,
///   set message->queueID to EV_NO_ID, then return.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_find_message_by_sender(EV_ID queue_id, UINT32 senderInfo, EV_Message *message);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_find_message_by_content
/// \~english @par Brief  get message event(search with contents)
/// \~english @param  [in] queue_id
///       ID of message event queue
/// \~english @param  [in] bytes
///       size of compare bytes
/// \~english @param  [in] compare_bytes
///       bytes that used to campare the contents
/// \~english @param  [out] message
///       message event struct
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @par
///         EV_Message struct
///   @code
///         #define EV_MAX_MESSAGE_LENGTH 2048
///         typedef struct {
///            EV_ID queueID;
///            UINT32 senderInfo;
///            UINT32 length;
///            UINT32 dummy;
///            UINT8  message[EV_MAX_MESSAGE_LENGTH];
///          } EV_Message;
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: The specified queue ID is not existing,
///   or it is not created in current process.
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Not find contents's message according to input contents information
///          - EV_ERR_Invalid_ID
///       - Securing free space of thread information management table (malloc) Failure
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Message queue pool is NULL
///       - Message queue generation corresponding to queue_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   If the event in compare bytes is same with
///   any event in message event queue whose ID is specified,
///   get the event and delete it from queue.
///   If such a message is not existing, set message->queueID to EV_NO_ID, then return.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_find_message_by_content(EV_ID queue_id,                 // NOLINT (readability/nolint)
                                  UINT16 length,                  // NOLINT (readability/nolint)
                                  const void *compare_bytes,      // NOLINT (readability/nolint)
                                  /* OUT */EV_Message *message);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_flag_fd
/// \~english @par Brief
///       Obtain fd for polling flag event
/// \~english @param [in] flag_id
///       EV_ID        - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] fd
///       int *        - event flag queue fd for Polling
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @par Prerequisite
///       Execute EV_create_flag(),EV_create_flag_auto_id() and flag has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       --incorrect flag_id:The flag_id of event which has not been created.
///       - Flag corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets fd(File Descriptor) corresponding to flag of ID specified by argument.
///       The user can only get flag created by user process .
///       This fd(File Descriptor) can be used only for designation to poll / select, and operation when other
///       operations are performed is not guaranteed.
///       At the time of poll, only POLLIN can be specified, and at the time of select,
///       it is possible to check only whether it can be read.
///       Also, if flag is deleted in poll / select, it exits poll / select. * In case of poll, POLLERR is set.
///       When calling the same ID more than once, it returns the same fd(File Descriptor)
///       each time (unless destroy / create is done).
/// \~english @see
///       nothing
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_flag_fd(EV_ID flag_id, /* OUT */int *fd);  // NOLINT (readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_get_queue_fd
/// \~english @par Brief
///       Obtain fd for polling message event
/// \~english @param [in] flag_id
///       EV_ID        - Flag ID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @param [out] fd
///       int * - Message queue fd for Polling
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error
///       - Message queue corresponding to queue_id doesn't exist
///       --incorrect queue_id:The queue_id of essage which has not been created.
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Gets fd(File Descriptor) corresponding to message queue of ID specified by argument.
///       The user can only get message queue created by user process .
///       This fd(File Descriptor) can be used only for designation to poll / select,
///       and operation when other operations are performed is not guaranteed.
///       At the time of poll, only POLLIN can be specified, and at the time of select,
///       it is possible to check only whether it can be read.
///       Also, if message queue is deleted in poll / select, it exits poll / select.
///       * In case of poll, POLLERR is set.
///       When calling the same ID more than once, it returns the same fd(File Descriptor)
///       each time (unless destroy / create is done).
/// \~english @see
///       nothing
////////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_get_queue_fd(EV_ID queue_id, /* OUT */int *fd);  // NOLINT (readability/nolint)

/* \~english Functions that specify the module ID (16 bits) instead of the flag ID and queue ID */

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_moduleID_to_flag_id
/* \~english function group used to specify moduleID(16bit) instead of flag_id, queue_id */
/// \~english @par Brief change from moduleID to flag_id
/// \~english @param  [in] m_id
///       moduleID
/// \~english @retval flag_id
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///    - none
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of flags with one moduleID. return error:EV_ERR_Exist.
///   This function is only used for generating one event by one module ID.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ID EV_moduleID_to_flagID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_moduleID_to_flag64ID
/// \~english @par Brief change from moduleID to 64bit flag_id
/// \~english @param  [in] m_id
///       moduleID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval 64bit flag_id
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///    - none
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of flags with one moduleID. return error:EV_ERR_Exist.
///   This function is only used for generating one event by one module ID.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ID EV_moduleID_to_flag64ID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_moduleID_to_queueID
/// \~english @par Brief change from moduleID to queue_id
/// \~english @param  [in] m_id
///       moduleID
/// \~english @par
///         EV_ID type parameter
///   @code
///         typedef UINT32 EV_ID
///         @endcode
/// \~english @retval queue_id
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///    - none
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of queues with one moduleID.
// \~english @see none
////////////////////////////////////////////////////////////////////////////////

EV_ID EV_moduleID_to_queueID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag_by_mID
/// \~english @par Brief create flag(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist Duplication Error
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The ID is not created by function of EV_moduleID_to_flagID ().
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - flag_id registered in launch thread
///       - Flag corresponding to flag_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of flags with one moduleID.return error:EV_ERR_Exist.
///   This function is only used for generating one event by one module ID.
// \~english @see EV_create_flag
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag_by_mID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_flag64_by_mID
/// \~english @par Brief create 64bit flag(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist Duplication Error
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The ID is not created by function of EV_moduleID_to_flag64ID ().
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - flag_id registered in launch thread
///       - Flag corresponding to flag_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Flag pool is NULL
///       - Flag generation corresponding to flag_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of flags with one moduleID. return error:EV_ERR_Exist.
///   This function is only used for generating one event by one module ID.
// \~english @see EV_create_flag64
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_create_flag64_by_mID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_create_queue_by_mID
/// \~english @par Brief create queue(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @param  [in] length
///       length of message queue
/// \~english @param  [in] max_bytes
///       one max bytes for message
/// \~english @param  [in] type
///       treat type of receiving events even though the queue is full
/// \~english @par
///         ev_message_queue_type struct
///   @code
///             enum ev_message_queue_type {
///               EV_MESSAGE_QUEUE_TYPE_BUSY,
///               EV_MESSAGE_QUEUE_TYPE_FIFO,
///               EV_MESSAGE_QUEUE_TYPE_REPLACE,
///             };
///         @endcode
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: set en invalid flag ID
/// \~english @retval EV_ERR_Thread_Over The MAX of event thread.
/// \~english @retval EV_ERR_Exist: a same flag ID is existing
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///       There is no prerequisite
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///          - EV_ERR_Invalid_ID
///       - Startup thread upper limit (16) error
///          - EV_ERR_Thread_Over
///       - queue_id registered in launch thread
///       - Message queue corresponding to queue_id already exists
///          - EV_ERR_Exist
///       - Securing free space of thread information management table (malloc) Failure
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Message queue pool is NULL
///       - Message queue generation corresponding to queue_id failed
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of queues with one moduleID.
// \~english @see EV_create_queue
////////////////////////////////////////////////////////////////////////////////

EV_ERR
EV_create_queue_by_mID(UINT16 m_id,                  // NOLINT (readability/nolint)
                      UINT8 length,                  // NOLINT (readability/nolint)
                      UINT16 max_bytes,              // NOLINT (readability/nolint)
                      EV_Message_Queue_Type type);   // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_destroy_flag_by_mID
/// \~english @par Brief delete flag(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @par length [in]length of message queue
/// \~english @par max_bytes [in]one max bytes for message
/// \~english @par type [in]treat type of receiving events even though the queue is full
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: set en invalid flag ID
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    - none
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The flag_id of event/message which has not been created.
///       - Flag corresponding to queue_id doesn't exist
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
/// \~english You can not use this function to
/// \~english create a multiple of flags with one moduleID. return error:EV_ERR_Exist.
/// This function is only used for generating one event by one module ID.
// \~english @see EV_destroy_flag
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_destroy_flag_by_mID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_destroy_queue_by_mID
/// \~english @par Brief delete queue(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Message queue corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///   You can not use this function to
///   create a multiple of queues with one moduleID
// \~english @see EV_destroy_queue
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_destroy_queue_by_mID(UINT16 m_id);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_set_flag_by_mID
/// \~english @par Brief send flag(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @param  [in] bits
///       value of flag
/// \~english @retval EV_OK: normal termination
/// \~english @retval EV_ERR_Invalid_ID: set en invalid flag ID
/// \~english @retval EV_ERR_Fatal: Fatal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The flag_id of event which has not been created.
///       - Flag corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sets bit pattern specified by argument to flag of ID specified by argument.
///       When setting a plurality of bit patterns in the same flag,
///       "only OR operation result of all bit patterns" is held in flag.
// \~english @see EV_set_flag
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_set_flag_by_mID(UINT16 m_id, UINT32 bits);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_set_flag64_by_mID
/// \~english @par Brief send 64bit flag(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @param  [in] bits
///       value of flag
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid flag_id
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///    - none
/// \~english @par Change of internal state
///    There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect flag_id)
///       --incorrect flag_id:The flag_id of event which has not been created.
///       - Flag corresponding to flag_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sets bit pattern specified by argument to flag of ID specified by argument.
///       When setting a plurality of bit patterns in the same flag, "only
///       OR operation result of all bit patterns" is held in flag.
// \~english @see EV_set_flag64
////////////////////////////////////////////////////////////////////////////////

EV_ERR EV_set_flag64_by_mID(UINT16 m_id, UINT64 bits);  // NOLINT (readability/nolint)

////////////////////////////////////////////////////////////////////////////////
/// \ingroup EV_send_message_by_mID
/// \~english @par Brief send message(moduleID specified)
/// \~english @param  [in] m_id
///       moduleID
/// \~english @param  [in] bytes
///       number of send bytes
/// \~english @param  [in] message
///       pointer to sender contents
/// \~english @param  [in] senderInfo
///       sender information
/// \~english @retval EV_OK Successful completion
/// \~english @retval EV_ERR_Invalid_ID Invalid queue_id
/// \~english @retval EV_ERR_Busy Queue overflow
/// \~english @retval EV_ERR_Fatal Abnormal error
/// \~english @par Prerequisite
///       Execute EV_create_queue(),EV_create_queue_auto_id() and message queue has already been created.
/// \~english @par Change of internal state
///       There is no change of the internal Status
/// \~english @par Conditions of processing failure
///       - Input parameter error (specifying incorrect queue_id)
///       --incorrect queue_id:The queue_id of essage which has not been created.
///       - Message queue corresponding to queue_id doesn't exist
///          - EV_ERR_Invalid_ID
///       - Queue overflow in kernel space
///          - EV_ERR_Busy
///       - Access failed from kernel space to user space reference area
///       - Failed to copy data from user space to kernel space
///       - Invalid queue types
///          - EV_ERR_Fatal
/// \~english @par Classification
///       Public
/// \~english @par Type
///       Sync Only
/// \~english @par Detail
///       Sends message specified by argument to message moduleID specified by argument.
///       An error occurs when message queue of the specified ID doesn't exist
///       or message size exceeds the specified size.
///       When message queue is FULL, the processing differs depending on the type
///       of message queue specified at the time of creation message queue.
/// \~english @see evk_open, evk_close, evk_ioctl, evk_store_message
////////////////////////////////////////////////////////////////////////////////

EV_ERR
EV_send_message_by_mID(UINT16 m_id,         // NOLINT (readability/nolint)
                      UINT16 bytes,         // NOLINT (readability/nolint)
                      const void *message,  // NOLINT (readability/nolint)
                      UINT32 senderInfo);   // NOLINT (readability/nolint)

/** @}*/  // end of event_library
/** @}*/  // end of other_service
/** @}*/  // end of BaseSystem
/**
 *
 * @brief Method to specify module IDs as flag IDs and queue IDs
 *
 * The function with module ID as an argument directly can be used in the case of that created only 
 * one flag and queue in the module, other than using macros EV_Flag_ID_Base() or EV_Queue_ID_Base().
 * @see EV_create_flag_by_mID
 * @see EV_create_queue_by_mID
 * @see EV_destroy_flag_by_mID
 * @see EV_destroy_queue_by_mID
 * @see EV_set_flag_by_mID
 * @see EV_send_message_by_mID
 *
 * So the following API can be used to convert IDs created using above method to flag IDs or queue ID
 * when pass them to other modules.
 * @see EV_moduleID_to_flagID(UINT16 m_id)
 * @see EV_moduleID_to_queueID(UINT16 m_id)
 *
 * @see EV_ID_spec
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__KERNEL__ */

#endif  // OTHERSERVICE_EV_LIB_H_
