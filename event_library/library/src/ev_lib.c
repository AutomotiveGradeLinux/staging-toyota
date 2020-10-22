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
 * @file ev_lib.c
 * @brief Events Library -- Implementing API Functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <other_service/ev_lib.h>
#include <agldd/evk_lib.h>
#include <agldd/ev_common.h>
#include <assert.h>
#define EV_perror perror
#define EV_assert assert

/*
 * Not use dbg_output in the EV so that the DTV apps and semiconductor vendor use this function.
 */
#define ev_log(format, arg...) \
{ \
  { \
    char procname[33]; \
    prctl(PR_GET_NAME, procname); \
    int pid = getpid(); \
    printf(format, procname, pid, ##arg); \
  } \
}

#define EV_DEBUG_INFO(format, arg...) \
{ \
  if (getenv("EV_DEBUG") != NULL) { \
    ev_log("[EV: info](%s:%d) " format, ##arg); \
  } \
}

#define EV_DEBUG_WARNING(format, arg...) \
{ \
  ev_log("[EV: warning](%s:%d) " format, ##arg); \
}

#define EV_DEBUG_ERR(format, arg...) \
{ \
  ev_log("[EV: ERR](%s:%d) " format, ##arg); \
}

/** @brief Structures for managing ID and FD in use by processes
 */
typedef struct {
    EV_ID id; /**< flag/queue ID */
    INT32 fd; /**< file descriptor */
}
EV_Info;

/*----------------------------------------*/
/** @brief EV management information table per thread */
/*----------------------------------------*/
typedef struct {
    /** Thread calling the EV library */
    pthread_t    thread;
    /** Registration information by ID */
    EV_Info      ev_info[EV_MAX_IDS_IN_THREAD];
    /** Number of registered IDs */
    UINT8        num;
}
EV_thread_info_type;

/** Thread information acquisition mode */
#define EV_THREAD_INFO_CREATE      0
#define EV_THREAD_INFO_PEEK        1
#define EV_SEARCH_ALL_THREAD       -1

/* Related process exclusion control in the process  */
const char *g_ev_lock_func;
int g_ev_lock_line;
int g_ev_lock_pid;
pthread_mutex_t g_ev_process_global_mutex = PTHREAD_MUTEX_INITIALIZER;

#define EV_PROCESS_MUTEX_LOCK \
    pthread_mutex_lock(&g_ev_process_global_mutex); \
    g_ev_lock_func = __FUNCTION__; \
    g_ev_lock_line = __LINE__; \
    g_ev_lock_pid = getpid();
#define EV_PROCESS_MUTEX_UNLOCK                        \
    pthread_mutex_unlock(&g_ev_process_global_mutex);

/** @mainpage
 * This document describes the following specifications:
 * - Event Library (EV) API specifications
 * - Event ID specifications
 *
 * In principle, the API specifications of the event library for kernel space (EVK) is
 * the same as the event library for user space (EV) except for the function name prefixed ""EVK"".
 * If there are differences between the EV and EVK, it is described as "difference in kernel version".
 */
/** @defgroup Event Library (EV) API specifications */
/** @defgroup EV_ID_spec Event ID specifications */
/** @defgroup EV_m_id Function specifying module ID instead of event ID */
/** @defgroup EV_in internal specifications of the event library for user space (EV)  */
/** @defgroup EVK_in Internal specifications of the event library for kernel space (EVK) */
/** @addtogroup EV_in
 * @{ */

/** EV thread information management table */
EV_thread_info_type  *EV_thread_info[EV_MAX_THREADS_IN_PROCESS];

/*----------------------------------------------------------------------
 * Return the registered position of the thread ID
 * Allocate and initialize space for unregistered threads
 * @brief Sending message (specified module-ID)
 * @param index [OUT] Thread index
 * @param flag  [IN] Operating Mode
 ----------------------------------------------------------------------*/
EV_ERR
EV_thread_id_check(int *index, int flag) {
  int i, empty;

  /* Current threadID */
  pthread_t EV_self_thread_id;

  /* Get thread ID */
  EV_self_thread_id = pthread_self();

  /* Control-process lock */
  EV_PROCESS_MUTEX_LOCK;

  /* Whether or not the thread has already been registered. */
  for(i = 0, empty = -1; i < EV_MAX_THREADS_IN_PROCESS; i++) {
    /* Free space */
    if(EV_thread_info[i] == NULL) {
      /* Set free space index */
      empty = (empty == -1) ? i: empty;
      continue;
    }

    if(EV_thread_info[i]->thread == EV_self_thread_id) {
      *index = i;

      /* Cancel in-process mutual exclusion */
      EV_PROCESS_MUTEX_UNLOCK;
      return EV_OK;
    }
  }

  if(flag == EV_THREAD_INFO_PEEK) {
    EV_DEBUG_ERR("thread id is not found.\n");

    /* Cancel in-process mutual exclusion */
    EV_PROCESS_MUTEX_UNLOCK;
    return EV_ERR_Invalid_Thread;
  }

  /* If there are no unregistered threads and there is no space */
  if(empty == -1) {
    EV_DEBUG_ERR("thread id is full.\n");

    /* Cancel in-process mutual exclusion */
    EV_PROCESS_MUTEX_UNLOCK;
    return EV_ERR_Thread_Over;
  }

  /* Set to free space */
  EV_thread_info[empty] = malloc(sizeof(*(EV_thread_info[empty])));
  EV_thread_info_type *th = EV_thread_info[empty];

  /* malloc failed */
  if(th == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function malloc
    /* Cancel in-process mutual exclusion */
    // LCOV_EXCL_START 5: fail safe for glibc function malloc

    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

    EV_PROCESS_MUTEX_UNLOCK;
    return EV_ERR_Fatal;
  }
  // LCOV_EXCL_STOP

  /* Initialization */
  memset(th, '\0', sizeof(*th));
  th->thread = EV_self_thread_id;
  /* th->num = 0; */

  /* Cancel in-process mutual exclusion */
  EV_PROCESS_MUTEX_UNLOCK;

  /* Set registration position */
  *index = empty;

  return EV_OK;
}

// free thread information when create event/message fail and no release thread buffer.
// If no release thread buffer will cause be error:
// Creation event's Thread count num less than MAX count(16) when one thread create event/message
// fail and no longer to create event/message.
static void
free_thread_buffer(int th_index) {
  /* in-process mutual exclusion */
  EV_PROCESS_MUTEX_LOCK;
  if ((th_index >= 0)
    &&(th_index < EV_MAX_THREADS_IN_PROCESS)
    && (NULL != EV_thread_info[th_index])
    && (0 == EV_thread_info[th_index]->num)) {
    free(EV_thread_info[th_index]);
    EV_thread_info[th_index] = NULL;
    EV_DEBUG_ERR("Free thread buffer by th_index:%d.\n", th_index);
  }
  /* in-process mutual exclusion */
  EV_PROCESS_MUTEX_UNLOCK;
}

/* EV_info update function */
static void
add_ev_info(int th_index, EV_ID id, int fd) {
  /* Check storage position of ID information */
  /* beginning of in-process mutual exclusion */
  EV_PROCESS_MUTEX_LOCK;

  int id_index = EV_thread_info[th_index]->num;
  EV_assert(id_index < EV_MAX_IDS_IN_THREAD);

  /* Set information */
  EV_thread_info[th_index]->ev_info[id_index].id = id;
  EV_thread_info[th_index]->ev_info[id_index].fd = fd;
  EV_thread_info[th_index]->num++;

  /* end of in-process mutual exclusion */
  EV_PROCESS_MUTEX_UNLOCK;
}

/* EV_info search function */
static int
find_ev_info(int th_index, EV_ID id) {
  int i, j, start, end;
  int ret = -1;

  /* Search all threads */
  if(th_index == EV_SEARCH_ALL_THREAD) {
    start = 0;
    end = EV_MAX_THREADS_IN_PROCESS;
  } else {
    start = th_index;
    end = th_index + 1;
  }

  /* in-process mutual exclusion */
  EV_PROCESS_MUTEX_LOCK;

  for(j = start; j < end; j++) {
    if(EV_thread_info[j] == NULL) {
      continue;
    }

    for(i = 0; i < EV_thread_info[j]->num; i++) {
      if(id == EV_thread_info[j]->ev_info[i].id) {
        ret = EV_thread_info[j]->ev_info[i].fd;
        break;
      }
    }
  }

  /* Cancel in-process mutual exclusion */
  EV_PROCESS_MUTEX_UNLOCK;

  return ret;
}

static int
delete_ev_info(EV_ID id) {
  int fd = -1;
  int i, j;
  int found_th_index = -1;
  int found_idx = -1;
  EV_thread_info_type *th = NULL;

  /* beginning of in-thread mutual exclusion */
  EV_PROCESS_MUTEX_LOCK;

  for(i = 0 ; i < EV_MAX_THREADS_IN_PROCESS && found_idx == -1 ; i++) {
    th = EV_thread_info[i];
    if (th == NULL) {
      continue;
    }
    for(j = 0 ; j < th->num ; j++) {
      if (th->ev_info[j].id == id) {
      fd = th->ev_info[j].fd;
      found_th_index = i;
      found_idx = j;
      break;
      }
    }
  }

  if(found_idx == -1) {
    EV_PROCESS_MUTEX_UNLOCK;
    return -1;
  }

  if (found_idx < th->num - 1) { /* pgr0060 */
    memmove(&(th->ev_info[found_idx]), &(th->ev_info[found_idx+1]),
      sizeof(th->ev_info[0]) * (size_t)(th->num - 1 - found_idx));
  }
  th->num--;

  /* If everything is deleted */
  if(th->num == 0 && EV_thread_info[found_th_index] != NULL) {
    free(EV_thread_info[found_th_index]);
    EV_thread_info[found_th_index] = NULL;
  }
  EV_PROCESS_MUTEX_UNLOCK;
  return fd;
}

/** @} */
/** @addtogroup EV
 * @{ */

static EV_ERR
EV_create_flag_in(int th_index, EV_ID flag_id, int is64bit) {
  int fd;

  /* Open device driver */
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  int ret = ioctl(fd, is64bit ? EVK_IOC_CREATE_FLAG64:
        EVK_IOC_CREATE_FLAG, flag_id);
  int myerrno = errno;

  if(ret < 0) {
    if(myerrno == EEXIST) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function ioctl
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      close(fd);  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
      EV_DEBUG_ERR("flag_id %x already exists.\n", flag_id);  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
      return EV_ERR_Exist;  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
    } else {
      close(fd);
      errno = myerrno;
      return EV_ERR_Fatal;
    }
  }

  ret = ioctl(fd, EVK_IOC_SET_POLL, flag_id);
  if(ret < 0) {
    close(fd);
    EV_DEBUG_ERR("set_poll: internal error\n");
    return EV_ERR_Fatal;
  }

  add_ev_info(th_index, flag_id, fd);
  return EV_OK;
}

/** @brief Create flag
 *
 * Create a flag with the given ID.
 * Return error if a flag with the same flag ID already exists
 *
 * @param flag_id [IN]flag ID
 * @see EV_ID
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Specified an invalid flag ID
 * @retval EV_ERR_Exist: A flag with the same ID already exists
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_create_flag (EV_ID flag_id) {
  EV_ERR err;
  int th_index;

  /* Flag ID check */
  if(!EV_ID_IS_FLAG(flag_id) || EV_ID_IS_AUTO_ID(flag_id)) {
    EV_DEBUG_ERR("create_flag: Invalid flag_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    return err;
  }

  EV_DEBUG_INFO("EV_create_flag (th_index=%d)\n", th_index);

  /* case of already created */
  int fd = find_ev_info(th_index, flag_id);
  if(fd >= 0) {
    EV_DEBUG_ERR("create_flag: You already created flag_id %x.\n", flag_id);
    return EV_ERR_Exist;
  }

  err = EV_create_flag_in(th_index, flag_id, 0);
  if (EV_OK != err) {
    free_thread_buffer(th_index);
  }

  return err;
}

/** @brief Create 64-bit flag
 *
 * Create a 64-bit flag with the given ID.
 * Return error if a flag with the same flag ID already exists
 *
 * @param flag_id [IN]64-bit flag ID
 * @see EV_ID
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Specified an invalid 64-bit flag ID
 * @retval EV_ERR_Exist: A 64-bit flag with same ID already exists
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_create_flag64(EV_ID flag_id) {
  EV_ERR err;
  int th_index;

  if(!EV_ID_IS_FLAG64(flag_id) || EV_ID_IS_AUTO_ID(flag_id)) {
    EV_DEBUG_ERR("create_flag: Invalid flag_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    return err;
  }

  int fd = find_ev_info(th_index, flag_id);
  if(fd >= 0) {
    EV_DEBUG_ERR("create_flag: You already created flag_id %x.\n", flag_id);
    return EV_ERR_Exist;
  }

  err =  EV_create_flag_in(th_index, flag_id, 1);
  if (EV_OK != err) {
    free_thread_buffer(th_index);
  }

  return err;
}

/** @brief Create flag (auto-assign ID)
 *
 * Automatically assign a flag ID and creates a flag.
 * Same as EV_create_flag(), except that ID is automatically assigned.
 *
 * @param flag_id [OUT]Area for storing the assigned flag ID
 *
 * @see EV_create_flag
 */
EV_ERR
EV_create_flag_auto_id(EV_ID *flag_id) {
  EV_assert(flag_id != NULL);

  EV_ERR err;
  int fd, th_index;
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  int ret = ioctl(fd, EVK_IOC_ALLOC_FLAG_ID, flag_id);
  close(fd);
  EV_assert(ret == 0);

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    *flag_id = EV_NO_ID;
    return err;
  }

  err = EV_create_flag_in(th_index, *flag_id, 0);
  if (err != EV_OK) {
    free_thread_buffer(th_index);

    *flag_id = EV_NO_ID;
    return EV_ERR_Fatal;
  }
  return EV_OK;
}

/** @brief Create 64-bit flag(auto-assign ID)
 *
 * Automatically assign a 64-bit flag ID and creates a 64-bit flag.
 * Same as EV_create_flag64(), except that ID is automatically assigned.
 *
 * @param flag_id [OUT]Area for storing the assigned 64-bit flag ID
 *
 * @see EV_create_flag64
 */
EV_ERR
EV_create_flag64_auto_id(EV_ID *flag_id) {
  EV_assert(flag_id != NULL);

  EV_ERR err;
  int fd, th_index;
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  int ret = ioctl(fd, EVK_IOC_ALLOC_FLAG64_ID, flag_id);
  close(fd);
  EV_assert(ret == 0);

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    *flag_id = EV_NO_ID;
    return err;
  }

  err = EV_create_flag_in(th_index, *flag_id, 1);
  if (err != EV_OK) {
    free_thread_buffer(th_index);

    *flag_id = EV_NO_ID;
    return EV_ERR_Fatal;
  }
  return EV_OK;
}

static EV_ERR
EV_create_queue_in(int th_index, EV_ID queue_id, UINT8 length,
  UINT16 max_bytes, EV_Message_Queue_Type type) {
  int fd;
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  EVK_Message_Queue_Request req;
  req.queueID = queue_id;
  req.length = length;
  req.max_bytes = max_bytes;
  req.type = type;

  int ret = ioctl(fd, EVK_IOC_CREATE_MESSAGE_QUEUE, &req);
  int myerrno = errno;
  if (ret < 0) {
    if (myerrno == EEXIST) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function ioctl
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      close(fd);  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
      EV_DEBUG_ERR("queue %x already exists.\n", queue_id);  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
      return EV_ERR_Exist;  // LCOV_EXCL_LINE 5: fail safe for glibc function ioctl
    } else {
      close(fd);
      errno = myerrno;
      //EV_perror("create queue");
      return EV_ERR_Fatal;
    }
  }

  ret = ioctl(fd, EVK_IOC_SET_POLL, queue_id);
  if (ret < 0) {
    close(fd);
    EV_DEBUG_ERR("set_poll: internal error\n");
    return EV_ERR_Fatal;
  }

  add_ev_info(th_index, queue_id, fd);
  return EV_OK;
}

/** @brief message event queue creation
 *
 * Create a queue of message event with the specified ID.
 * Return error if a queue with the same ID already exists.
 *
 * A memory area corresponding to the specified maximum number of bytes x queue length is allocated dynamically.
 *
 * @param queue_id [IN] Message queue ID
 * @param length [IN] Message queue length
 * @param max_bytes [IN] - Maximum number of bytes per message
 * @param type [IN] Type of action to take when more events are received when the queue is full
 * @see EV_ID
 * @see ev_message_queue_type
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Specified an invalid flag ID
 * @retval EV_ERR_Exist: A flag with the same ID already exists
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_create_queue(EV_ID queue_id, UINT8 length, UINT16 max_bytes,
    EV_Message_Queue_Type type) {
    EV_ERR err;
    int th_index;

  if(!EV_ID_IS_QUEUE(queue_id) || EV_ID_IS_AUTO_ID(queue_id)) {
    EV_DEBUG_ERR("create_queue: Invalid queue_id %x was specified.\n",
        queue_id);
    return EV_ERR_Invalid_ID;
  }

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    return err;
  }

  int fd = find_ev_info(th_index, queue_id);
  if(fd >= 0) {
    EV_DEBUG_ERR("create_queue: You already created queue_id %x.\n",
        queue_id);
    return EV_ERR_Exist;
  }

  err = EV_create_queue_in(th_index, queue_id, length, max_bytes, type);
  if (EV_OK != err) {
    free_thread_buffer(th_index);
  }

  return err;
}

/** @brief message event queue creation(auto-assign ID)
 *
 * Automatically assigns a queue ID and creates a queue for message events.
 * Same as EV_create_queue(), except that ID is automatically assigned.
 *
 * @param queue_id [OUT] Area for storing the assigned flag ID
 * @param length [IN] Message queue length
 * @param max_bytes [IN] - Maximum number of bytes per message
 * @param type [IN] Type of action to take when more events are received when the queue is full
 *
 * @see EV_create_queue
 */
EV_ERR
EV_create_queue_auto_id(EV_ID *queue_id, UINT8 length, UINT16 max_bytes,
      EV_Message_Queue_Type type) {
      int th_index;

  EV_assert(queue_id != NULL);
  EV_ERR err;
  int fd;
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  int ret = ioctl(fd, EVK_IOC_ALLOC_QUEUE_ID, queue_id);
  close(fd);
  EV_assert(ret == 0);

  /* Thread information check */
  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_CREATE);
  if(err != EV_OK) {
    *queue_id = EV_NO_ID;
    return err;
  }

  err = EV_create_queue_in(th_index, *queue_id, length, max_bytes,
          type);
  if (err != EV_OK) {
    free_thread_buffer(th_index);

    *queue_id = EV_NO_ID;
    return EV_ERR_Fatal;
  }
  return EV_OK;
}

/** @brief Deleting flag event
 *
 * Delete the flag with the specified ID.
 * Only flags created by this process can be deleted.
 *
 * - differences in kernel versions
 * EVK_destroy_flag can be used to delete any existing message event queue.
 *
 * @note The program wakes up that waiting for this queue (poll or wait) 
 * due to deletion.
 *
 * @param flag_id [IN] ID of flag
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_destroy_flag(EV_ID flag_id) {
  int fd = delete_ev_info(flag_id);
  if (fd < 0) {
    EV_DEBUG_ERR("You haven't created flag/queue_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  int ret = ioctl(fd, EVK_IOC_DESTROY_QUEUE, flag_id);
  int myerrno = errno;
  close(fd);
  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_ERR("destroy: no such ID %x\n", flag_id);
      return EV_ERR_Invalid_ID;
    } else {
      errno = myerrno;
      EV_perror("destroy");
      return EV_ERR_Fatal;
    }
  }
  return EV_OK;
}

/** @brief Delete message event queue
 *
 * Delete the message event queue with the specified ID.
 * Only message event queues created by this process can be deleted.
 *
 * - differences in kernel versions
 * EVK_destroy_queue can be used to delete any existing message event queue.
 *
 * @note The program wakes up that waiting for this queue (poll or wait) 
 * due to deletion.
 *
 * @param flag_id [IN] Message event queue ID
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The message queue with the specified ID does not exist,
 * or there is no message event queue created by this process.
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_destroy_queue(EV_ID queue_id) {
  return EV_destroy_flag(queue_id);
}

static EV_ERR
EV_get_flag_in(EV_ID flag_id, EV_Flag *flag, int peek_only, int wait) {
  EV_assert(flag != NULL);
  flag->flagID = EV_NO_ID;
  flag->bits = 0;

  if (!EV_ID_IS_FLAG(flag_id)) {
    EV_DEBUG_ERR("get_flag: %x is not a flag_id.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  int fd = find_ev_info(EV_SEARCH_ALL_THREAD, flag_id);
  if(fd < 0) {
    EV_DEBUG_ERR("You have not created flag_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  EV_Event ev;
  ev.type = EV_EVENT_Flag;
  ev.u.flag.flagID = flag_id;
  ev.u.flag.bits = 0;
  int ret = ioctl(
    fd,
    (peek_only ? EVK_IOC_PEEK_EVENT : (wait ? EVK_IOC_WAIT_EVENT : EVK_IOC_GET_EVENT)),
    &ev);

  int myerrno = errno;
  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_ERR("get_flag: No such flag_id %x.\n", flag_id);
      return EV_ERR_Invalid_ID;
    }
    else if (myerrno == EINTR) {
      EV_DEBUG_ERR("get_flag: Interrupted\n");
      return EV_ERR_Interrupted;
    } else {
      errno = myerrno;
      EV_perror("get flag");
      return EV_ERR_Fatal;
    }
  }

  if (ev.type == EV_EVENT_Flag) {
    flag->flagID = ev.u.flag.flagID;
    flag->bits = ev.u.flag.bits;
  }
  return EV_OK;
}

static EV_ERR
EV_get_flag64_in(EV_ID flag_id, EV_Flag64 *flag, int peek_only, int wait) {
  EV_assert(flag != NULL);
  flag->flagID = EV_NO_ID;
  flag->bits = 0;

  if (!EV_ID_IS_FLAG64(flag_id)) {
    EV_DEBUG_ERR("get_flag: %x is not a flag_id.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  int fd = find_ev_info(EV_SEARCH_ALL_THREAD, flag_id);
  if (fd < 0) {
    EV_DEBUG_ERR("You have not created flag_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  EV_Event ev;
  ev.type = EV_EVENT_Flag64;
  ev.u.flag64.flagID = flag_id;
  ev.u.flag64.bits = 0;
  int ret = ioctl(
    fd,
    (peek_only ? EVK_IOC_PEEK_EVENT : (wait ? EVK_IOC_WAIT_EVENT : EVK_IOC_GET_EVENT)),
    &ev);

  int myerrno = errno;
  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_ERR("get_flag: No such flag_id %x.\n", flag_id);
      return EV_ERR_Invalid_ID;
    }
    else if (myerrno == EINTR) {
      EV_DEBUG_ERR("get_flag: Interrupted\n");
      return EV_ERR_Interrupted;
    } else {
      errno = myerrno;
      EV_perror("get flag");
      return EV_ERR_Fatal;
    }
  }

  if (ev.type == EV_EVENT_Flag64) {
    flag->flagID = ev.u.flag64.flagID;
    flag->bits = ev.u.flag64.bits;
  }
  return EV_OK;
}

/** @brief Get flag event(non-block)
 *
 * Get the flag event with the specified ID, and clears the flag.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be used to determine no events.
 * Only flags created by this process can be gotten.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] Flag Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_flag(EV_ID flag_id, EV_Flag *flag) {
  return EV_get_flag_in(flag_id, flag, 0, 0);
}

/** @brief Get 64bit flag event(non-block)
 *
 * Get the 64-bit flag event with the specified ID, and clears the 64-bit flag.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be used to determine no events.
 * Only flags created by this process can be gotten.
 * Be sure to specify the flag ID created by specifying 64-bits.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] 64-bit flag struct
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_flag64(EV_ID flag_id, EV_Flag64 *flag) {
  return EV_get_flag64_in(flag_id, flag, 0, 0);
}

/** @brief Get flag event(block)
 *
 * Get the flag event with the specified ID, and clear the flag.
 * If there is no event, block until an event is received.
 * However, if the interrupt is interrupted by a signal interrupt, EV_ERR_Interrupted is returned.
 * If the flag is deleted while waiting, the wait is suspended 
 * and EV_ERR_Invalid_ID is returned.
 * When interrupted, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be judged to have been interrupted.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] Flag Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval EV_ERR_Interrupted: Interrupted by a signal interrupt
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_wait_flag(EV_ID flag_id, EV_Flag *flag) {
  return EV_get_flag_in(flag_id, flag, 0, 1);
}

/** @brief Get 64bit flag event(block)
 *
 * Get the 64-bit flag event with the specified ID, and clear the 64-bit flag.
 * If there is no event, block until an event is received.
 * However, if the interrupt is interrupted by a signal interrupt, EV_ERR_Interrupted is returned.
 * If the flag is deleted while waiting, the wait is suspended 
 * and EV_ERR_Invalid_ID is returned.
 * When interrupted, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be judged to have been interrupted.
 * Be sure to specify the flag ID created by specifying 64 bits.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] 64-bit flag structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval EV_ERR_Interrupted: Interrupted by a signal interrupt
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_wait_flag64(EV_ID flag_id, EV_Flag64 *flag) {
  return EV_get_flag64_in(flag_id, flag, 0, 1);
}

/** @brief Get flag event(non-destructive)
 *
 * Get the flag event with the specified ID non-destructively.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be used to determine no events.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] Flag Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_peek_flag(EV_ID flag_id, EV_Flag *flag) {
  return EV_get_flag_in(flag_id, flag, 1, 0);
}

/** @brief Get 64bit flag event(non-destructive)
 *
 * Gets the 64-bit flag event with the specified ID non-destructively.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in flag->flagID.
 * A flag->bits of 0 can also be used to determine no events.
 *
 * @param flag_id [IN] Message event queue ID
 * @param flag [OUT] 64-bit flag structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist,
 * Or it is not a flag created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_peek_flag64(EV_ID flag_id, EV_Flag64 *flag) {
  return EV_get_flag64_in(flag_id, flag, 1, 0);
}

static EV_ERR
EV_get_message_in(EV_ID queue_id, EV_Message *message, UINT32 senderInfo,
      UINT32 length, const void *compare_bytes,
      int peek_only, int wait) {
  EV_assert(message != NULL);
  message->queueID = EV_NO_ID;
  message->senderInfo = 0;
  message->length = 0;

  if (!EV_ID_IS_QUEUE(queue_id)) {
    EV_DEBUG_ERR("get_message: Invalid queue_id %x\n", queue_id);
    return EV_ERR_Invalid_ID;
  }

  int fd = find_ev_info(EV_SEARCH_ALL_THREAD, queue_id);
  if(fd < 0) {
    EV_DEBUG_ERR("You have not created queue_id %x.\n", queue_id);
    return EV_ERR_Invalid_ID;
  }

  EV_Event ev;
  ev.type = EV_EVENT_Message;
  ev.u.message.queueID = queue_id;
  ev.u.message.senderInfo = senderInfo;
  if(length > 0 && compare_bytes != NULL) {
    ev.u.message.length = length;
    memcpy(ev.u.message.message, compare_bytes, length);
  } else {
    ev.u.message.length = 0;
  }

  int ret = ioctl(
    fd,
    (peek_only ? EVK_IOC_PEEK_EVENT : (wait ? EVK_IOC_WAIT_EVENT : EVK_IOC_GET_EVENT)),
    &ev);

  int myerrno = errno;
  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_ERR("No such queue_id %x\n", queue_id);
      return EV_ERR_Invalid_ID;
    }
    else if (myerrno == EINTR) {
      EV_DEBUG_INFO("Interrupted.\n");
      return EV_ERR_Interrupted;
    } else {
      errno = myerrno;
      EV_perror("get message");
      return EV_ERR_Fatal;
    }
  }

  if(ev.type == EV_EVENT_Message) {
    message->queueID = ev.u.message.queueID;
    message->senderInfo = ev.u.message.senderInfo;
    message->length = ev.u.message.length;
    memcpy(message->message, ev.u.message.message,
      ev.u.message.length);
  }

  return EV_OK;
}

/** @brief Get message events(non-block)
 *
 * Get one message event from the queue with the specified ID.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in message-> queue_id.
 * The acquired message is deleted from the queue.
 *
 * @param queue_id [IN] Message event queue ID
 * @param message [OUT] Message Event Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Queue with specified ID does not exist
 * Or not a message event queue created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_message(EV_ID queue_id, EV_Message *message) {
  return EV_get_message_in(queue_id, message, 0, 0, NULL, 0, 0);
}

/** @brief Get message event(block)
 *
 * Get one message event from the queue with the specified ID.
 * If there is no event, block until an event is received.
 * However, if the interrupt is interrupted by a signal interrupt, EV_ERR_Interrupted is returned.
 * If the queue is deleted while waiting, the waiting is suspended,
 * EV_ERR_Invalid_ID is returned.
 * When interrupted, EV_NO_ID is stored in message-> queue_id.
 *
 * @param queue_id [IN] Message event queue ID
 * @param message [OUT] Message Event Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Queue with specified ID does not exist
 * Or not a message event queue created by invoking process
 * @retval EV_ERR_Interrupted: Interrupted by a signal interrupt
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_wait_message(EV_ID queue_id, EV_Message *message) {
  return EV_get_message_in(queue_id, message, 0, 0, NULL, 0, 1);
}

/** @brief Get message event(non-destructive)
 *
 * Get one message event queue of the specified ID non-destructively.
 * Even if there is no event, it returns immediately.
 * If there are no events, EV_NO_ID is stored in message-> queue_id.
 * Same as EV_get_message(), except that the retrieved message remains in the queue.
 *
 * @param queue_id [IN] Message event queue ID
 * @param message [OUT] Message Event Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Queue with specified ID does not exist
 * Or not a message event queue created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_peek_message(EV_ID queue_id, EV_Message *message) {
  return EV_get_message_in(queue_id, message, 0, 0, NULL, 1, 0);
}

static EV_ERR
EV_get_next_event_in(EV_Event *ev, int peek_only) {
  int i, th_index;
  EV_ERR err;
  EVK_Next_Event_Query query;

  EV_assert(ev != NULL);
  ev->type = EV_EVENT_None;

  bzero(&query, sizeof(query));

  err = EV_thread_id_check(&th_index, EV_THREAD_INFO_PEEK);
  if(err != EV_OK) {
    return err;
  }

  EV_PROCESS_MUTEX_LOCK;
  query.num = EV_thread_info[th_index]->num;
  for(i = 0 ; i < query.num ; i++) {
    query.ids[i] = EV_thread_info[th_index]->ev_info[i].id;
  }
  EV_PROCESS_MUTEX_UNLOCK;

  if (query.num == 0) {
    return EV_OK;
  }

  int fd;
  fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  int ret = ioctl(
    fd,
    (peek_only ? EVK_IOC_PEEK_NEXT_EVENT: EVK_IOC_GET_NEXT_EVENT),
    &query);

  int myerrno = errno;
  close(fd);

  if (ret < 0) {
    errno = myerrno;
    EV_perror("get_next_event");
    return EV_ERR_Fatal;
  }
  memcpy(ev, &(query.ev), sizeof(*ev));
  return EV_OK;
}

/** @brief Get the first event that arrived(non-block)
 *
 * Get the first arrival of a flag created by invoking process or an event 
 * that occurred in the message queue.
 * If there are no events, EV_EVENT_None is stored in the ev->type and returned immediately.
 *
 * @param ev [OUT] Event Structure
 * Whether the acquired event type is flag or message is determined 
 * by ev->type is EV_EVENT_Flag or EV_EVENT_Message.
 *
 * @retval EV_OK: Normal completion
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_next_event(EV_Event *ev) {
  return EV_get_next_event_in(ev, 0);
}

/** @brief Get the first event that arrived(non-destructive)
 *
 * Get the first arrival of a flag created by invoking process or an event 
 * that occurred in the message queue non-destructively.
 * The next call to EV_get_next_event() or EV_peek_next_event(),
 * return the same event as long as no other program gets it.
 * If there are no events, EV_EVENT_None is stored in the ev->type and returned immediately.
 *
 * @param ev [OUT] Event Structure
 * Whether the acquired event flag or message is a message is determined 
 * by whether ev->type is EV_EVENT_Flag or EV_EVENT_Message.
 *
 * @retval EV_OK: Normal completion
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_peek_next_event(EV_Event *ev) {
  return EV_get_next_event_in(ev, 1);
}

/** @brief Get message event(search by source info)
 *
 * If there is an event whose source information matches  
 * in the message event queue of the specified ID,it is gotten and deleted from the queue.
 * If there are no such messages, 
 * EV_NO_ID is stored in message->queue_id, and the system call returns immediately.
 *
 * @param queue_id [IN] Message event queue ID
 * @param senderInfo [IN] Source Info
 * @param message [OUT] Message Event Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Queue with specified ID does not exist
 * Or not a message event queue created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_find_message_by_sender(EV_ID queue_id, UINT32 senderInfo,
        EV_Message *message) {
  EV_assert(message != NULL);
  EV_assert(senderInfo != 0);
  return EV_get_message_in(queue_id, message, senderInfo, 0, NULL, 0, 0);
}

/** @brief Get message event(search by content)
 *
 * If there is an event with byte sequence matching the specified bytes from the beginning 
 * in the message event queue of the specified ID,it is gotten and deleted from the queue.
 * If there are no such messages,
 * EV_NO_ID is stored in message->queue_id, and the system call returns immediately.
 *
 * @param queue_id [IN] Message event queue ID
 * @param bytes [IN] Bytes to compare
 * @param compare_bytes [IN] Byte sequence to compare content against
 * @param message [OUT] Message Event Structure
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Queue with specified ID does not exist
 * Or not a message event queue created by invoking process
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_find_message_by_content(EV_ID queue_id,
         UINT16 bytes, const void *compare_bytes,
         EV_Message *message) {
  EV_assert(message != NULL);
  EV_assert(bytes <= EV_MAX_MESSAGE_LENGTH);
  EV_assert(compare_bytes != NULL);

  return EV_get_message_in(queue_id, message, 0, bytes,
          compare_bytes, 0, 0);
}

/** @brief Set flag event
 *
 * Set the flag with the specified ID.
 *
 * @param queue_id [IN] Flag ID
 * @param bits [IN] Flag bit-pattern
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist.
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_set_flag(EV_ID flag_id, UINT32 bits) {
  if (!EV_ID_IS_FLAG(flag_id)) {
    EV_DEBUG_ERR("set_flag: %x is not a valid flag_id.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  int fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  EV_Flag flag;
  flag.flagID = flag_id;
  flag.bits = bits;

  int ret = ioctl(fd, EVK_IOC_STORE_FLAG, &flag);
  int myerrno = errno;
  close(fd);

  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_WARNING("set_flag: No such flag_id %x.\n", flag_id);
      return EV_ERR_Invalid_ID;
    }
    errno = myerrno;
    EV_perror("set flag");
    return EV_ERR_Fatal;
  }
  return EV_OK;
}

/** @brief Set 64bit flag event
 *
 * Set the flag with the specified ID.
 *
 * @param queue_id [IN] Flag ID
 * @param bits [IN] Bit pattern of 64-bit flag
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: The flag with the specified ID does not exist.
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_set_flag64(EV_ID flag_id, UINT64 bits) {
  if (!EV_ID_IS_FLAG64(flag_id)) {
    EV_DEBUG_ERR("set_flag: %x is not a valid flag_id.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  int fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
  EV_assert(fd >= 0);

  EV_Flag64 flag;
  flag.flagID = flag_id;
  flag.bits = bits;

  int ret = ioctl(fd, EVK_IOC_STORE_FLAG64, &flag);
  int myerrno = errno;
  close(fd);

  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_ERR("set_flag: No such flag_id %x.\n", flag_id);
      return EV_ERR_Invalid_ID;
    }

    errno = myerrno;
    EV_perror("set flag");
    return EV_ERR_Fatal;
  }

  return EV_OK;
}

/** @brief Send Messaging Event
 *
 * Send a message event to the specified queue ID.
 *
 * @param queue_id [IN] Queue ID
 * @param bytes [IN] Bytes to send
 * @param buf [IN] Pointer to the byte sequence to send
 * @param senderInfo [IN] Source info
 * Specify this option to identify applications, etc.
 * When not used, specify 0.
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: Message event queue with specified ID does not exist
 * @retval EV_ERR_Busy: Queue overflow
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_send_message(EV_ID queue_id, UINT16 bytes, const void *buf, UINT32 senderInfo) {
  EV_assert(buf != NULL);
  EV_assert(bytes <= EV_MAX_MESSAGE_LENGTH);
  if (!EV_ID_IS_QUEUE(queue_id)) {
    EV_DEBUG_ERR("send_message: %x is not a valid queue_id.\n", queue_id);
    return EV_ERR_Invalid_ID;
  }


  /*
   * Open fd at the first time of send_message in process, 
   * and continue to use the fd with it open after that.
   * Reason:
   * - Prevent frequent use of open/close system calls.
   * - If the process is nonresident, the fd is close after the process is terminated.
   * - Even for resident processes, there is no problem because processes 
   *   that use event library normally continue to use event library 
   *   from the beginning to the end.
   */
  static int fd = -1;
  EV_PROCESS_MUTEX_LOCK;
  if (fd < 0) {
    fd = open(EVK_DEV_NAME, O_RDWR|O_CLOEXEC);
    if (fd < 0) {  // LCOV_EXCL_BR_LINE 5: fail safe for glibc function open
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert

      EV_PROCESS_MUTEX_UNLOCK;  // LCOV_EXCL_LINE 5: fail safe for glibc function open
      return EV_ERR_Fatal;  // LCOV_EXCL_LINE 5: fail safe for glibc function open
    }
    fcntl(fd, F_SETFD, FD_CLOEXEC|(fcntl(fd, F_GETFD, 0)));
  }

  EV_Message msg;
  msg.queueID = queue_id;
  msg.senderInfo = senderInfo;
  msg.length = bytes;
  memcpy(msg.message, buf, bytes);

  int ret = ioctl(fd, EVK_IOC_STORE_MESSAGE, &msg);
  int myerrno = errno;
  EV_PROCESS_MUTEX_UNLOCK;

  if (ret < 0) {
    if (myerrno == ENOENT) {
      EV_DEBUG_WARNING("send_message: No such queue_id %x.\n", queue_id);
      return EV_ERR_Invalid_ID;
    } else if (myerrno == EBUSY) {
      EV_DEBUG_WARNING("send_message: queue_id %x returned BUSY.\n", queue_id);
      return EV_ERR_Busy;
    }

    errno = myerrno;
    EV_perror("send message");
    return EV_ERR_Fatal;
  }

  return EV_OK;
}

/** @brief Get fd for poll flag event
 *
 * Get file descriptor (FD) that used to wait for a flag 
 * with the specified ID in a poll/select.
 * Only flags created by invoked process can be specified.
 *
 * This FD can only be used to specify a poll/select.
 * Subsequent operations are not guaranteed if any other operations are performed.
 * When use poll, only POLLIN can be specified in events.
 * When use select, you can only check if it is readable.
 *
 * If the same flag_id is called more than once,
 * The same FD is returned each time (unless there is a destroy/create during that time).
 *
 * - differences in kernel versions
 * - This API is not provided in the EVK.
 *
 * @note If the flag is deleted during poll/select using the FD gotten by the API, 
 * the poll/select is exited.
 * POLLERR is set as event for poll.
 *
 * @param flag_id [IN] Flag ID
 * @param fd [OUT] Pointer to the area to store the file descriptor
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: An invalid flag ID was specified.
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_flag_fd(EV_ID flag_id, int *fd) {
  EV_assert(fd != NULL);
  *fd = -1;
  if ((!EV_ID_IS_FLAG(flag_id))&& (!EV_ID_IS_FLAG64(flag_id))) {
    EV_DEBUG_ERR("get_flag_fd: %x is not a valid flag_id.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  *fd = find_ev_info(EV_SEARCH_ALL_THREAD, flag_id);
  if (*fd < 0) {
    EV_DEBUG_ERR("You have not created flag_id %x.\n", flag_id);
    return EV_ERR_Invalid_ID;
  }

  return EV_OK;
}

/** @brief Get fd for message event queue poll
 *
 * Get file descriptor (FD) that used to wait for a queue 
 * with the specified ID in a poll/select.
 * Only message event queues created by invoked process can be specified.
 *
 * This FD can only be used to specify a poll/select.
 * Subsequent operations are not guaranteed if any other operations are performed.
 * When use poll, only POLLIN can be specified in events.
 * When use select, you can only check if it is readable.
 *
 * If the same queue_id is called more than once,
 * The same FD is returned each time (unless there is a destroy/create during that time).
 *
 * - differences in kernel versions
 * - This API is not provided in the EVK.
 *
 * @note If the queue is deleted during poll/select using the FD gotten by the API, 
 * the poll/select is exited.
 * POLLERR is set as event for poll.
 *
 * @param queue_id [IN] Message event queue ID
 * @param fd [OUT] Pointer to the area to store the file descriptor
 *
 * @retval EV_OK: Normal completion
 * @retval EV_ERR_Invalid_ID: An invalid flag ID was specified.
 * @retval Otherwise: FATAL error
 */
EV_ERR
EV_get_queue_fd(EV_ID queue_id, int *fd) {
  EV_assert(fd != NULL);
  *fd = -1;
  if (!EV_ID_IS_QUEUE(queue_id)) {
    EV_DEBUG_ERR("get_queue_fd: %x is not a valid queue_id.\n",
        queue_id);
    return EV_ERR_Invalid_ID;
  }

  *fd = find_ev_info(EV_SEARCH_ALL_THREAD, queue_id);
  if (*fd < 0) {
    EV_DEBUG_ERR("You have not created queue_id %x.\n", queue_id);
    return EV_ERR_Invalid_ID;
  }
  return EV_OK;
}

/* \~english Functions that specify the module ID (16 bits) instead of the flag ID and queue ID */
EV_ID EV_moduleID_to_flagID(UINT16 mID)
{
  return EV_Flag_ID_Base(mID);
}

EV_ID EV_moduleID_to_flag64ID(UINT16 mID)
{
  return EV_Flag64_ID_Base(mID);
}

EV_ID EV_moduleID_to_queueID(UINT16 mID)
{
  return EV_Queue_ID_Base(mID);
}

EV_ERR EV_create_flag_by_mID(UINT16 mID)
{
  return EV_create_flag(EV_moduleID_to_flagID(mID));
}

EV_ERR EV_create_flag64_by_mID(UINT16 mID)
{
  return EV_create_flag64(EV_moduleID_to_flag64ID(mID));
}

EV_ERR EV_create_queue_by_mID(UINT16 mID, UINT8 length,UINT16 max_bytes,
                       EV_Message_Queue_Type type)
{
  return EV_create_queue(EV_moduleID_to_queueID(mID), length, max_bytes, type);
}

EV_ERR EV_destroy_flag_by_mID(UINT16 mID)
{
  return EV_destroy_flag(EV_moduleID_to_flagID(mID));
}

EV_ERR EV_destroy_queue_by_mID(UINT16 mID)
{
  return EV_destroy_queue(EV_moduleID_to_queueID(mID));
}

EV_ERR EV_set_flag_by_mID(UINT16 mID, UINT32 bits)
{
  return EV_set_flag(EV_moduleID_to_flagID(mID), bits);
}

EV_ERR EV_set_flag64_by_mID(UINT16 mID, UINT64 bits)
{
  return EV_set_flag64(EV_moduleID_to_flag64ID(mID), bits);
}

EV_ERR EV_send_message_by_mID(UINT16 mID, UINT16 bytes, const void *message,
                              UINT32 senderInfo)
{
  return EV_send_message(EV_moduleID_to_queueID(mID),
			 bytes, message, senderInfo);
}
/** @} */
