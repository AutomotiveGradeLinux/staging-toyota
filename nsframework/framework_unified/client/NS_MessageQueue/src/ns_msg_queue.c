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
/// \ingroup  tag_NSMessageQueue
/// \brief    API Header for Zone Player Service APIs to be used by senders and
///           receivers.
///
/// APIs to register/unregister notifications and add/remove recievers to the notifications.
///
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "ns_msg_queue.h"
#include <malloc.h>
#include <mqueue.h>
#include <ns_mq_internal.h>
#include <errno.h>
#include "ns_mq_anomaly.h"

#include <other_service/strlcpy.h>
#include <other_service/strlcat.h>

const UI_32 DEFAULT_MSG_PRIORITY  = 10;
const UI_32 MAX_MESSAGES_STORED_IN_QUEUE = 256;
const UI_32 MAX_SYNC_RESPONSE_STORED_IN_QUEUE = 2;

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Function : TranslateError
//  Translates global error variables into FW EFrameworkunifiedStatus
////////////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus TranslateError(int error) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;

  switch (error) {
    case EOK:
      eStatus = eFrameworkunifiedStatusOK;
      break;
    case EBUSY:
      eStatus = eFrameworkunifiedStatusThreadBusy;
      break;
    case EDEADLK:
      eStatus = eFrameworkunifiedStatusThreadSelfJoin;
      break;
    case EFAULT:
      eStatus = eFrameworkunifiedStatusFault;
      break;
    case EINVAL:
      eStatus = eFrameworkunifiedStatusInvldParam;
      break;
    case ESRCH:
      eStatus = eFrameworkunifiedStatusThreadNotExist;
      break;
    case EBADF:
      eStatus = eFrameworkunifiedStatusErrNoEBADF;
      break;
    case EAGAIN:
      eStatus = eFrameworkunifiedStatusErrNoEAGAIN;
      break;
    case EINTR:
      eStatus = eFrameworkunifiedStatusErrNoEINTR;
      break;
    case EMSGSIZE:
      eStatus = eFrameworkunifiedStatusInvldBufSize;
      break;
    case ENOTSUP:
      eStatus = eFrameworkunifiedStatusNotImplemented;
      break;
    case EPERM:
      eStatus = eFrameworkunifiedStatusAccessError;
      break;
    default:
      eStatus = eFrameworkunifiedStatusFail;
      break;
  }

  return eStatus;
}

static UI_8 GetNormalizedMqName(PSTR normalized_mqname , PCSTR name, size_t size) {
  if ((NULL != name) && (NULL != normalized_mqname)) {
    if (name[0] != '/') {
      strlcpy(normalized_mqname, "/", size);
      strlcat(normalized_mqname, name, size);
    } else {
      strlcpy(normalized_mqname, name, size);
    }

    return 1;
  } else {
    return 0;
  }
}

HANDLE OpenReceiverNotBlocked(PCSTR name) {
  HANDLE rtnHandle = INVALID_HANDLE;  // invalid until made valid.
  PSTR normalized_qname = NULL;
  SI_32 q_fd = -1;  // fd to mqueue
  SQhandle *rcvHndl = NULL;
  struct mq_attr mqattr;
  size_t norm_qname_size = 0;

  // Check for invalid name
  if (name == NULL) {
    return INVALID_HANDLE;
  }

  if (strlen(name) >= MAX_QUEUE_NAME_SIZE) {
    return INVALID_HANDLE;
  }

  norm_qname_size = strlen(name) + 2;
  normalized_qname = (PSTR)malloc(norm_qname_size);

  if (0 != GetNormalizedMqName(normalized_qname, name, norm_qname_size)) {
    if (strlen(normalized_qname) > LIMIT_QUEUE_NAME_SIZE) {
      free(normalized_qname);
      return INVALID_HANDLE;
    }

    mqattr.mq_flags = 0;
    mqattr.mq_maxmsg = (__syscall_slong_t)MAX_MESSAGES_STORED_IN_QUEUE;
    mqattr.mq_msgsize = MAX_QUEUE_MSG_SIZE;

    q_fd = mq_open(normalized_qname, O_RDONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC , 0666, &mqattr);

    if (q_fd != -1) {
      rcvHndl = (SQhandle *)malloc(sizeof(SQhandle));
      if (rcvHndl != NULL) {
        rcvHndl->check_code = MQ_CHECK_CODE;
        rcvHndl->fd = q_fd;
        rcvHndl->q_name = normalized_qname;
        rcvHndl->q_type = eQTypeReveiver;

        /////////////////////////////////////////
        rcvHndl->threadid = 0;
        /////////////////////////////////////////

        // Set the return handle to rcvHndl
        rtnHandle = rcvHndl;
      }
    }
  }

  if (INVALID_HANDLE == rtnHandle) {
    // couldn't connect the queue,
    // release the memory to normalized queue name
    if (normalized_qname != NULL) {
      free(normalized_qname);
      normalized_qname = NULL;  // mb20110108  item 11
    }
  }

  return rtnHandle;
}

static HANDLE openReceiverInternal(PCSTR name, UI_32 mq_maxmsg, UI_32 mq_msgsize, BOOL is_internal) {
  HANDLE rtnHandle = INVALID_HANDLE;  // invalid until made valid.
  PSTR normalized_qname = NULL;
  SI_32 q_fd = -1;  // fd to mqueue
  SQhandle *rcvHndl = NULL;
  struct mq_attr mqattr;
  size_t norm_qname_size = 0;

  // Check for invalid name
  if (name == NULL) {
    return INVALID_HANDLE;
  }

  if (strlen(name) >= MAX_QUEUE_NAME_SIZE) {
    return INVALID_HANDLE;
  }

  norm_qname_size = strlen(name) + 2;
  normalized_qname = (PSTR)malloc(norm_qname_size);

  if (0 != GetNormalizedMqName(normalized_qname, name, norm_qname_size)) {
    int i;

    if (!is_internal && strlen(normalized_qname) > LIMIT_QUEUE_NAME_SIZE) {
      free(normalized_qname);
      return INVALID_HANDLE;
    }

    mqattr.mq_flags = 0;
    mqattr.mq_maxmsg = (__syscall_slong_t)mq_maxmsg;
    mqattr.mq_msgsize = (__syscall_slong_t)mq_msgsize;

    for (i = 0; i < (sizeof(mq_anomaly_list) / sizeof(mq_anomaly_list[0])); i++) {
      if (strcmp(normalized_qname, mq_anomaly_list[i].name) == 0) {
        mqattr.mq_maxmsg = (__syscall_slong_t)mq_anomaly_list[i].maxMsg;
        break;
      }
    }


    q_fd = mq_open(normalized_qname, O_RDONLY | O_CREAT | O_CLOEXEC, 0666, &mqattr);

    if (q_fd != -1) {
      rcvHndl = (SQhandle *)malloc(sizeof(SQhandle));
      if (rcvHndl != NULL) {
        rcvHndl->check_code = MQ_CHECK_CODE;
        rcvHndl->fd = q_fd;
        rcvHndl->q_name = normalized_qname;
        rcvHndl->q_type = eQTypeReveiver;

        /////////////////////////////////////////
        rcvHndl->threadid = 0;
        /////////////////////////////////////////

        // Set the return handle to rcvHndl
        rtnHandle = rcvHndl;
      }

    }
  }

  if (INVALID_HANDLE == rtnHandle) {
    // couldn't connect the queue,
    // release the memory to normalized queue name
    if (normalized_qname != NULL) {
      free(normalized_qname);
      normalized_qname = NULL;  // mb20110108  item 11
    }
  }

  return rtnHandle;
}

HANDLE OpenReceiver(PCSTR name) {
  return openReceiverInternal(name, MAX_MESSAGES_STORED_IN_QUEUE, MAX_QUEUE_MSG_SIZE, FALSE);
}

HANDLE openSyncReceiver(PCSTR name) {
  return openReceiverInternal(name, MAX_SYNC_RESPONSE_STORED_IN_QUEUE , MAX_QUEUE_MSG_SIZE, TRUE);
}

static HANDLE openSenderInternal(PCSTR name, UI_32 mq_maxmsg, UI_32 mq_msgsize, BOOL is_internal, BOOL zero_copy) {
  HANDLE rtnHandle = INVALID_HANDLE;  // invalid until made valid.
  SI_32 q_fd = -1;  // fd to queue
  SQhandle *sndHndl = NULL;
  PSTR normalized_qname = NULL;
  size_t norm_qname_size = 0;

  struct mq_attr mqattr;

  // Check for invalid name
  if (name == NULL) {
    return rtnHandle;
  }

  if (strlen(name) >= MAX_QUEUE_NAME_SIZE) {
    return rtnHandle;
  }

  norm_qname_size = strlen(name) + 2;
  normalized_qname = (PSTR)malloc(norm_qname_size);

  if (NULL == normalized_qname) {
    return rtnHandle;
  }

  if (0 != GetNormalizedMqName(normalized_qname, name, norm_qname_size)) {
    int i;


    if (!is_internal && strlen(normalized_qname) > LIMIT_QUEUE_NAME_SIZE) {
      free(normalized_qname);
      return INVALID_HANDLE;
    }

    mqattr.mq_flags = 0;
    mqattr.mq_maxmsg = (__syscall_slong_t)mq_maxmsg;
    mqattr.mq_msgsize = (__syscall_slong_t)mq_msgsize;

    for (i = 0; i < (sizeof(mq_anomaly_list) / sizeof(mq_anomaly_list[0])); i++) {
      if (strcmp(normalized_qname, mq_anomaly_list[i].name) == 0) {
        mqattr.mq_maxmsg = (__syscall_slong_t)mq_anomaly_list[i].maxMsg;
        break;
      }
    }

    q_fd = mq_open(normalized_qname, O_WRONLY | O_CREAT | O_NONBLOCK | O_CLOEXEC, 0666, &mqattr);

    if (q_fd != -1) {
      sndHndl = (SQhandle *)malloc(sizeof(SQhandle));
      if (NULL != sndHndl) {
        sndHndl->check_code = MQ_CHECK_CODE;
        sndHndl->fd = q_fd;
        sndHndl->q_name = normalized_qname;
        sndHndl->q_type = eQTypeSender;

        sndHndl->threadid = 0;


        // Set the return handle to sndHndl
        rtnHandle = sndHndl;

        if (zero_copy) {
          sndHndl->sendbuf = malloc(MAX_QUEUE_MSG_SIZE);
          if (sndHndl->sendbuf == NULL) {
            // LCOV_EXCL_START 5: malloc's error case
            AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
            free(sndHndl);
            rtnHandle = INVALID_HANDLE;
            // LCOV_EXCL_STOP
          }
        } else {
          sndHndl->sendbuf = NULL;
        }
      }
    }
  }

  if (INVALID_HANDLE == rtnHandle) {
    // couldn't connect the queue,
    // release the memory to normalized queue name
    if (normalized_qname != NULL) {
      free(normalized_qname);
      normalized_qname = NULL;  // mb20110108  item 11
    }
    if (q_fd != -1) {
      mq_close(q_fd);
    }
  }

  return rtnHandle;  // invalid until made valid.
}

HANDLE OpenSender(PCSTR name) {
  return openSenderInternal(name, MAX_MESSAGES_STORED_IN_QUEUE, MAX_QUEUE_MSG_SIZE, FALSE, FALSE);
}


HANDLE OpenSenderChild(PCSTR name, pthread_t threadid) {
  HANDLE h = OpenSender(name);
  if (INVALID_HANDLE != h) {
    SQhandle *sq = (SQhandle *)h;
    sq->threadid = threadid;
  }
  return h;
}


HANDLE openSyncSender(PCSTR name) {
  return openSenderInternal(name, MAX_SYNC_RESPONSE_STORED_IN_QUEUE, MAX_QUEUE_MSG_SIZE, TRUE, FALSE);
}

HANDLE openSenderZc(PCSTR name) {
  return openSenderInternal(name, MAX_MESSAGES_STORED_IN_QUEUE, MAX_QUEUE_MSG_SIZE, FALSE, TRUE);
}


EFrameworkunifiedStatus JoinChild(HANDLE hChildApp) {
  // mb20110108 Code re-structured per  comments 24 & 25
  if (mqCheckValidHandle(hChildApp)) {
    SQhandle *sq = (SQhandle *)hChildApp;
    return TranslateError(pthread_join(sq->threadid, NULL));
  } else {
    return eFrameworkunifiedStatusFail;
  }
}

EFrameworkunifiedStatus GetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusFail;
  // LCOV_EXCL_BR_START 6:GetChildThreadPriority is called by McGetChildThreadPriority, and hChildApp will check there.
  if (mqCheckValidHandle(hChildApp)) {
  // LCOV_EXCL_BR_STOP
    SQhandle *sq = (SQhandle *)hChildApp;
    SI_32 schedPolicy;  // this is needed according to syntax of pthread_getschedparam. api not available to get prio directly.
    struct sched_param schedParam;
    eStatus = TranslateError(pthread_getschedparam(sq->threadid, &schedPolicy, &schedParam));

    *threadPrio = schedParam.sched_priority;
  } else {
    // LCOV_EXCL_START 6:GetChildThreadPriority is called by McGetChildThreadPriority, and hChildApp will check there.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }
  return eStatus;
}

/// endhack...
/////////////////////////////////////////

EFrameworkunifiedStatus SendMessage(HANDLE hMessage, UI_32 length, PVOID data) {
  return SendMessageWithPriority(hMessage, length, data, eFrameworkunifiedMsgPrioNormal);
}

EFrameworkunifiedStatus SendMessageWithPriority(HANDLE hMessage, UI_32 length, PVOID data, EFrameworkunifiedMessagePriorties priority) {
  // define a Q handle structure
  SQhandle *sndHndl = NULL;

  // mb20110108 Added per  comments 27 & 28
  if (length > 0 && NULL == data) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  // check handle for null case...
  if (mqCheckValidHandle(hMessage) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  sndHndl = (SQhandle *)hMessage;

  // check to see if this is a sender handle
  if (sndHndl->q_type != eQTypeSender) {
    return eFrameworkunifiedStatusInvldHndlType;
  }

  if (-1 == mq_send((mqd_t)sndHndl->fd, (PCSTR)data, (size_t)length, (unsigned int)priority)) {
    if (errno == EAGAIN) {
      return eFrameworkunifiedStatusMsgQFull;
    } else {
      return TranslateError(errno);
    }
  }
  return eFrameworkunifiedStatusOK;
}

SI_32  ReceiveMessage(HANDLE hMessage, UI_32 length, PVOID data) {
  // define a Q handle structure
  SQhandle *rcvHndl = NULL;

  // check handle for null case...
  if (mqCheckValidHandle(hMessage) == FALSE) {
    errno = ENODATA;
    return -1;
  }

  rcvHndl = (SQhandle *)hMessage;

  // check to see if this is a receiver handle
  if (rcvHndl->q_type != eQTypeReveiver) {
    errno = ENODATA;
    return -1;
  }


  return (SI_32)mq_receive((mqd_t)rcvHndl->fd, (char *)data, (size_t)length, NULL);
}

EFrameworkunifiedStatus CloseReceiver(HANDLE handle) {
  SQhandle *rcvHndl = NULL;
  SI_32 q_fd;

  // check handle for null case...
  if (mqCheckValidHandle(handle) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  rcvHndl = (SQhandle *)handle;

  // check to see if this is a receiver handle
  if (rcvHndl->q_type != eQTypeReveiver) {
    return eFrameworkunifiedStatusInvldHndlType;
  }

  rcvHndl->check_code = 0;
  q_fd = rcvHndl->fd;

  if (NULL != rcvHndl->q_name) {
    free(rcvHndl->q_name);    // remove the memory to the name
    rcvHndl->q_name = NULL;
  }
  free((void *)handle);    // remove handle.. now..
  handle = INVALID_HANDLE;

  return ((-1 == mq_close((mqd_t)q_fd)) ? eFrameworkunifiedStatusInvldHandle : eFrameworkunifiedStatusOK);
}

EFrameworkunifiedStatus CloseSender(HANDLE handle) {
  SQhandle *sndHndl = NULL;
  SI_32 q_fd;

  // check handle for null case...
  if (mqCheckValidHandle(handle) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  sndHndl = (SQhandle *)handle;

  // check to see if this is a sender handle
  if (sndHndl->q_type != eQTypeSender) {
    return eFrameworkunifiedStatusInvldHndlType;
  }

  sndHndl->check_code = 0;
  q_fd = sndHndl->fd;  // copy the fd, need it to close the queues....

  if (NULL != sndHndl->q_name) {
    free(sndHndl->q_name);  // remove the memory to the name
    sndHndl->q_name = NULL;
  }
  if (NULL != sndHndl->sendbuf) {
    free(sndHndl->sendbuf);
    sndHndl->sendbuf = NULL;
  }
  free((void *)handle);  // remove handle.. now..
  handle = INVALID_HANDLE;  // invalidate handle so user doesn't reuse...

  return (-1 == mq_close((mqd_t)q_fd)) ? eFrameworkunifiedStatusInvldHandle : eFrameworkunifiedStatusOK;
}

static UI_8 IsMessageAvailable(SI_32 fd) {
  struct mq_attr sMqStatus;
  if (-1 == mq_getattr(fd, &sMqStatus)) {
    // Error Detected.
    return 0;
  } else {
    if (0 < sMqStatus.mq_curmsgs) {
      return 1;
    } else {
      return 0;
    }
  }
}

void Flush(HANDLE hMessage) {
  SQhandle *rcvHndl = NULL;

  if (mqCheckValidHandle(hMessage)) {
    rcvHndl = (SQhandle *)hMessage;

    if (rcvHndl->q_type != eQTypeReveiver) {
      return;
    }

    // check there is anything on the queue before going into the loop....
    if (IsMessageAvailable(rcvHndl->fd)) {
      CHAR l_pReceiveBuffer_o[MAX_QUEUE_MSG_SIZE];

      // read till there isn't anything on the queue.
      while (IsMessageAvailable(rcvHndl->fd)) {
        mq_receive((mqd_t)rcvHndl->fd, l_pReceiveBuffer_o, (size_t)MAX_QUEUE_MSG_SIZE, NULL);
      }
    }
  }
}

EQType GetQueueType(HANDLE hMessage) {
  EQType qType = eQTypeInvld;

  if (mqCheckValidHandle(hMessage)) {
    SQhandle *handle = (SQhandle *)hMessage;
    qType = handle->q_type;
  }

  return qType;
}

PCSTR GetQueueName(HANDLE hMessage) {
  PCSTR name = NULL;

  if (mqCheckValidHandle(hMessage)) {
    SQhandle *hMsgQ = (SQhandle *)hMessage;

    if (hMsgQ->q_type == eQTypeSender ||
        hMsgQ->q_type == eQTypeReveiver) {
      name = hMsgQ->q_name;
    }
  }

  return name;
}

int GetQueueFD(HANDLE hMessage) {
  int fd = -1;

  if (mqCheckValidHandle(hMessage)) {
    SQhandle *hMsgQ = (SQhandle *)hMessage;

    if (hMsgQ->q_type == eQTypeSender ||
        hMsgQ->q_type == eQTypeReveiver) {
      fd = hMsgQ->fd;
    }
  }

  return fd;
}
