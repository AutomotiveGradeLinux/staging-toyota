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
/// \brief
///
/// APIs to Open\Close and Send\Receive on message queues and shared memory.
///
///////////////////////////////////////////////////////////////////////////////

#include <native_service/ns_message_center_if.h>
#include <ns_message_center_internal.h>
#include "ns_msg_queue.h"
#include <ns_mq_internal.h>
#include <ns_mc_internal.h>
#include <native_service/ns_msgs.h>
#include <string.h>
#include <stdarg.h>
#include <native_service/ns_shared_mem_if.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <sys/epoll.h>
#include <native_service/ns_transmit_log.h>
#include <native_service/ns_logger_if.h>
#include <native_service/ns_resource_controler.h>
#include <native_service/nsfw_message.h>
#include <limits.h>

#include <other_service/strlcpy.h>

#define assert_static(e) \
  do { \
    enum { assert_static__ = 1/(e) }; \
  } while (0)

#define MC_INVOKE_SYNC_TIMEOUT (60 * 1000)

typedef struct {
  UI_32 seq_id;
  EFrameworkunifiedStatus ret_val;
  UI_32 res_size;
} ResponseHeader;

static unsigned int mcSeed[4];

HANDLE McOpenReceiverNotBlocked(PCSTR name) {
  return OpenReceiverNotBlocked(name);
}

HANDLE McOpenReceiver(PCSTR name) {
  return OpenReceiver(name);
}

HANDLE McOpenSyncReceiver(PCSTR name) {
  return openSyncReceiver(name);
}


HANDLE McOpenSender(PCSTR name) {
  return OpenSender(name);
}

HANDLE McOpenSenderNotBlocked(PCSTR name) {
  return OpenSender(name);
}

HANDLE McOpenSyncSender(PCSTR name) {
  return openSyncSender(name);
}

static int mcSetMonitorName(char *str, PCSTR name) {
  int len;
  if (str == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "str is NULL");
    return -1;
  }
  *str = '\0';
  len = sprintf(str + 1, "monitorS_%s", name != 0 ? name : NULL);
  return len + 1;
}

static int OpenMonitor(PCSTR name) {
  int monitor_sock = -1;
  int monitor_sa_len;
  struct sockaddr_un monitor_sa_un;

  if ((monitor_sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "socket:%s", strerror(errno));
    goto error;
  }

  memset(&monitor_sa_un, 0, sizeof(monitor_sa_un));
  monitor_sa_un.sun_family = AF_UNIX;
  monitor_sa_len = mcSetMonitorName(monitor_sa_un.sun_path, name);
  monitor_sa_len += (int)offsetof(struct sockaddr_un, sun_path);

  if (bind(monitor_sock, (struct sockaddr *)&monitor_sa_un, (socklen_t)monitor_sa_len) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "bind:%s:%s", name, strerror(errno));  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    goto error;
  }

  if (listen(monitor_sock, SOMAXCONN) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "listen:%s", strerror(errno));  // LCOV_EXCL_BR_LINE 15:marco defined in "native_service/ns_logger_if.h"
    goto error;
  }
  return monitor_sock;

error:
  if (monitor_sock >= 0) {
    close(monitor_sock);
  }
  return -1;
}

int mcOpenMonitor(PCSTR name) {
  long monitor_sock;

  if (frameworkunifiedAcquireResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, name, &monitor_sock) < 0) {
    if ((monitor_sock = OpenMonitor(name)) != -1) {
      if (frameworkunifiedRegistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, name, monitor_sock, 1) < 0) {
        close(monitor_sock);
        monitor_sock = -1;
      }
    }
  }

  return (int)monitor_sock;
}

int mcCloseMonitor(PCSTR name) {
  long monitor_sock;

  if (frameworkunifiedGetResource(FRAMEWORKUNIFIED_RES_ABNMLMON, name, &monitor_sock) < 0) {
    return -1;
  }

  frameworkunifiedUnregistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, name);
  close((int)monitor_sock);

  return 0;
}

static int mcSetClientName(char *str, PCSTR serverName, PCSTR clientName) {
  int len;
  if (str == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "str is NULL");
    return -1;
  }
  *str = '\0';
  len = sprintf(str + 1, "monitorC_%s:%s", serverName != 0 ? serverName : NULL, clientName != 0 ? clientName : NULL);
  return len + 1;
}

static int ConnectMonitor(PCSTR serverName, PCSTR clientName) {
  int client_sock = -1;
  int client_sa_len, monitor_sa_len;
  struct sockaddr_un client_sa_un, monitor_sa_un;

  if ((client_sock = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) < 0) {  // LCOV_EXCL_BR_LINE 5: socket's error case
    // LCOV_EXCL_START 5:socket's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "socket:%s", strerror(errno));
    return -1;
    // LCOV_EXCL_STOP
  }

  memset(&client_sa_un, 0, sizeof(client_sa_un));
  client_sa_un.sun_family = AF_UNIX;
  client_sa_len = mcSetClientName(client_sa_un.sun_path, serverName, clientName);
  client_sa_len += (int)offsetof(struct sockaddr_un, sun_path);

  if (bind(client_sock, (struct sockaddr *)&client_sa_un, (socklen_t)client_sa_len) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "bind:%s:%s", client_sa_un.sun_path + 1, strerror(errno));
    goto error;
  }

  memset(&monitor_sa_un, 0, sizeof(monitor_sa_un));
  monitor_sa_un.sun_family = AF_UNIX;
  monitor_sa_len = mcSetMonitorName(monitor_sa_un.sun_path, serverName);
  monitor_sa_len += (int)offsetof(struct sockaddr_un, sun_path);

  if (connect(client_sock, (struct sockaddr *)&monitor_sa_un, (socklen_t)monitor_sa_len) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "connect:%s", strerror(errno));
    goto error;
  }
  return client_sock;

error:
  close(client_sock);
  return -1;
}

int mcSetConnectKey(char *buf, size_t size, PCSTR serverName, PCSTR clientName) {
  int len = snprintf(buf, size, "%s:%s", serverName != 0 ? serverName : NULL, clientName != 0 ? clientName : NULL);
  if (buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "buf is NULL");
    return -1;
  }
  buf[size - 1] = '\0';
  return len;
}

int mcConnectMonitor(PCSTR serverName, PCSTR clientName) {
  long client_sock;
  char connect_key[32];

  mcSetConnectKey(connect_key, sizeof(connect_key), serverName, clientName);

  if (frameworkunifiedAcquireResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key, &client_sock) < 0) {
    if ((client_sock = ConnectMonitor(serverName, clientName)) != -1) {
      if (frameworkunifiedRegistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key, client_sock, 1) < 0) {
        close(client_sock);
        client_sock = -1;
      }
    }
  }

  return (int)client_sock;
}

int mcCloseConnectMonitor(PCSTR serverName, PCSTR clientName) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  long client_sock;
  char connect_key[32];

  mcSetConnectKey(connect_key, sizeof(connect_key), serverName, clientName);

  if (frameworkunifiedGetResource(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key, &client_sock) == -1) {
    return -1;
  }

  if (frameworkunifiedReleaseResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key) == 0) {
    frameworkunifiedUnregistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key);
    close((int)client_sock);
  }

  return 0;
}
// LCOV_EXCL_STOP
static void mcGetClientName(const char *str, char *client_name, size_t size, int pathlen) {
  size_t len = size - 1;
  size_t str_len;
  if (str == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "str is NULL");
    return;
  }
  char *p_div = strchr(str + 1, ':');
  if (p_div == NULL) {
    return;
  }
  str_len = (size_t)(pathlen - (p_div - str + 1));
  if (len > str_len) {
    len = str_len;
  }
  if (client_name == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "client_name is NULL");
    return;
  }
  strncpy(client_name, p_div + 1, len);
  client_name[len] = '\0';
}

int mcAcceptMonitor(PCSTR serverName, int listenMonitorFd) {
  int accept_sock = -1;
  struct sockaddr_un client_sa_un;
  socklen_t client_sa_len = sizeof(client_sa_un);
  char client_name[16];
  char connect_key[32];

  if ((accept_sock = accept4(listenMonitorFd, (struct sockaddr *)&client_sa_un, &client_sa_len, SOCK_CLOEXEC)) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "accept:%s", strerror(errno));
    return -1;
  }

  mcGetClientName(client_sa_un.sun_path, client_name, sizeof(client_name), (int)(client_sa_len - offsetof(struct sockaddr_un,
                                                                                                          sun_path)));
  mcSetConnectKey(connect_key, sizeof(connect_key), serverName, client_name);

  frameworkunifiedRegistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key, accept_sock, 0);
  return accept_sock;
}

int mcGetAcceptMonitor(PCSTR serverName, PCSTR clientName) {
  long accept_sock;
  char connect_key[32];

  mcSetConnectKey(connect_key, sizeof(connect_key), serverName, clientName);

  if (frameworkunifiedAcquireResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key, &accept_sock) < 0) {
    return -1;
  }

  return (int)accept_sock;
}

int mcGetClientNameFromConnectKey(PCSTR connectKey, PCHAR clientName, size_t len) {
  char *p_div = strchr(connectKey, ':');
  if (p_div == NULL) {
    return -1;
  }

  strncpy(clientName, p_div + 1, len);
  clientName[len - 1] = '\0';
  return 0;
}

int mcGetServerNameFromConnectKey(PCSTR connectKey, PCHAR serverName, size_t len) {
  char *p_div = strchr(connectKey, ':');
  size_t min_len;

  if (p_div == NULL) {
    return -1;
  }

  min_len = ((len - 1) < (p_div - connectKey)) ? len - 1 : (size_t)(p_div - connectKey);
  strncpy(serverName, connectKey, min_len);
  serverName[min_len] = '\0';
  return 0;
}

/////////////////////////////////////////
HANDLE McOpenSenderChild(PCSTR name, pthread_t childid) {
  return OpenSenderChild(name, childid);
}

EFrameworkunifiedStatus McJoinChild(HANDLE hChildApp) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (INVALID_HANDLE != hChildApp) {  // LCOV_EXCL_BR_LINE 6: impossible to confirm because hChildApp checked is passed in the FrameworkunifiedDestroyChildThread function of the frameworkunified_multithreading.cpp
    eStatus = JoinChild(hChildApp);
  } else {
    // LCOV_EXCL_START 6: impossible to confirm because hChildApp checked is passed in the FrameworkunifiedDestroyChildThread function of the frameworkunified_multithreading.cpp
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusInvldHandle;
    // LCOV_EXCL_STOP
  }

  return eStatus;
}

EFrameworkunifiedStatus McGetChildThreadPriority(HANDLE hChildApp, PSI_32 threadPrio) {
  EFrameworkunifiedStatus eStatus = eFrameworkunifiedStatusOK;

  if (INVALID_HANDLE != hChildApp) {
    eStatus = GetChildThreadPriority(hChildApp, threadPrio);
  } else {
    eStatus = eFrameworkunifiedStatusInvldHandle;
  }

  return eStatus;
}

/// endhack...
/////////////////////////////////////////

static EFrameworkunifiedStatus mcReceiveInternal(HANDLE hMessage, PSTR source, UI_32 *cmd,  UI_32 *sessionid, UI_32 length,
                                    PVOID data) {
  PSSystemMsgHeader sysHdr = NULL;

  // check if handle is valid
  if (hMessage == INVALID_HANDLE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // check if buffer is null
  if (data == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  // check for invalid length.
  if (length <= 0) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  // check if the source is null
  if (source == NULL) {
    return eFrameworkunifiedStatusInvldQName;
  }

  if (cmd == NULL) {
    return eFrameworkunifiedStatusInvldParam;
  }

  if (-1 != ReceiveMessage(hMessage, length, data)) {
    sysHdr = (PSSystemMsgHeader)data;
    if (strlen(sysHdr->info.source) < MAX_QUEUE_NAME_SIZE) {
      *cmd = sysHdr->info.command;
      if (sessionid) {
        *sessionid = sysHdr->info.sessionid;
      }
      strlcpy(source, sysHdr->info.source, MAX_NAME_SIZE_APP);
      return eFrameworkunifiedStatusOK;
    } else {
      return eFrameworkunifiedStatusInvldBuf;
    }
  } else {
    return TranslateError(errno);
  }

  return eFrameworkunifiedStatusFail;
}

EFrameworkunifiedStatus McReceive(HANDLE hMessage, PSTR source, UI_32 *cmd, UI_32 length, PVOID data) {
  return mcReceiveInternal(hMessage, source, cmd, NULL, length, data);
}

EFrameworkunifiedStatus McReceiveWithSession(HANDLE hMessage, PSTR source, UI_32 *cmd,  UI_32 *sessionid, UI_32 length, PVOID data) {
  if (sessionid == NULL) {
    return eFrameworkunifiedStatusInvldParam;
  }
  return mcReceiveInternal(hMessage, source, cmd, sessionid, length, data);
}

static EFrameworkunifiedStatus mcSendInternal(HANDLE hMessage, PCSTR source, UI_32 cmd, PCSTR sysInfo, UI_32 length, PCVOID data,
                                 EFrameworkunifiedMessagePriorties priority, UI_32 sessionid, BOOL is_sync) {
  assert_static(_NSFW_MSG_LEN_ == sizeof(SSystemMsgHeader));
  assert_static(_NSFW_SYSINFO_SIZE_ == MAX_SYS_INFO_SIZE);
  assert_static(_NSFW_SYSINFO_FLAG_ == MC_EMSGT_SYSINFO_MASK);

  // check if handle is valid
  if (mqCheckValidHandle(hMessage) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // check if buffer is null
  if (data == NULL && length > 0) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  // check if the source is null
  if (source == NULL) {
    return eFrameworkunifiedStatusInvldQName;
  }

  // check if the source length exceeds than expected
  /**
   * @todo
   * Although a send message queue handle can be created using a 19-byte message queue name, messages cannot be sent using the created send message queue handle.
   * Because "/" is added to the queue name in the send message queue handle, a judgment error of MAX_QUEUE_NAME_SIZE (20 bytes) occurs during transmission and transmission cannot be performed.
   */
  if (strlen(source) >= MAX_QUEUE_NAME_SIZE) {
    return eFrameworkunifiedStatusInvldQName;
  }

  // notification name size should be less than MAX_SYS_INFO_SIZE
  // data length check
  UI_32 max_length = 0;
  if (NULL != sysInfo) {
    if (strlen(sysInfo) >= MAX_SYS_INFO_SIZE) {
      return eFrameworkunifiedStatusInvldParam;
    }
    // If there is an sysInfo, the maximum send data length is the maximum send data length of UI_32 minus (header + sysInfo size).
    max_length = 0xFFFFFFFF - sizeof(SSystemMsgHeader) - sizeof(TSysMsgSystemInfo);
  } else {
    // If there is no sysInfo, the maximum send data length is the maximum value of UI_32 minus the headers.
    max_length = 0xFFFFFFFF - sizeof(SSystemMsgHeader);
  }

  if (length > max_length) {
    return eFrameworkunifiedStatusInvldParam;
  }
  EFrameworkunifiedStatus rtnStatus = eFrameworkunifiedStatusOK;

  UI_8 sndBuf[ MAX_QUEUE_MSG_SIZE ];
  SQhandle *sndHndl = (SQhandle *)hMessage;
  SSystemMsgHeader *msgHdr = (SSystemMsgHeader *)sndBuf;
  UI_32 send_length = 0;
  UI_32 offset = sizeof(SSystemMsgHeader);

  if (NULL == sndHndl->q_name) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // check if the dest length exceeds than expected
  if (strlen(sndHndl->q_name) >= MAX_QUEUE_NAME_SIZE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // compose header
  strcpy(msgHdr->info.source, source);
  msgHdr->info.length = length;  // this is length of the messages data...
  msgHdr->info.command = cmd;
  msgHdr->info.sessionid = sessionid;

  msgHdr->eType = (UI_32)MC_EMSGT_VALID_TOKEN;
  msgHdr->memId = 0;  // not set...
  if (NULL != sysInfo) {
    msgHdr->eType |= MC_EMSGT_SYSINFO_MASK;
    strcpy((char *)(sndBuf + offset), sysInfo);
    offset += (UI_32)sizeof(TSysMsgSystemInfo);
  }
  if (is_sync) {
    msgHdr->eType |= MC_EMSGT_SYNC_MASK;
  }

  // Build buffer to send...

  // copy to buffer
  /*
   * @todo
   * Do not throw an error even if the data size of the argument is set to 0.
   * When "length + offset" is MAX_QUEUE_MSG_SIZE (4096) or more, the length becomes 0 in the process that is executed cannot be checked.
   * Sizeof(SSystemMsgHeader) is 40, sizeof(TSysMsgSystemInfo) is 64, maximal. offset is 104, min. is 40
   */
  if ((length + offset) >= MAX_QUEUE_MSG_SIZE) {
    // set the shared mem info in header....
    msgHdr->eType |= MC_EMSGT_SHM_MASK;

    // send data to shm
    msgHdr->memId = SetDataToShared(data, length, source, sndHndl->q_name);

    if (BAD_MEM_ID != msgHdr->memId) {
      // update the send length
      send_length += offset;
    } else {
      rtnStatus = eFrameworkunifiedStatusErrOther;
    }
  } else {  // was less than the max
    // update the send length
    send_length += offset;

    /**
     * @todo
     *  If UINT_MAX (4294967295) is specified in the iLength of FrameworkunifiedNPPublishNotification,
     *  the comparison by (length + offset) >= MAX_QUEUE_MSG_SIZE fails to compare correctly because of an overflow,
     *  and the comparison is judged to be less than the max.
     *  Once in the else block, the data is treated as a sysHeader block of up to 4096 bytes,
     *  and the memcpy is performed again without size checking, resulting in a segmentation fault.
     *  Maximum message length that can be sent by FrameworkunifiedNPPublishNotification must be specified and the maximum message length must be checked.
     */
    // copy the msg to send buffer
    memcpy(&sndBuf[0] + offset, data, length);

    // update the send length
    send_length += length;
  }

  if (eFrameworkunifiedStatusOK == rtnStatus) {
    rtnStatus = SendMessageWithPriority(hMessage, send_length, (PVOID)&sndBuf[0], priority);
  }

  if (rtnStatus != eFrameworkunifiedStatusOK) {
    // remove the shared mem since this failed.
    McClearData(msgHdr);
  }

  return rtnStatus;
}

EFrameworkunifiedStatus McSendPrioWithSysInfo(HANDLE hMessage, PCSTR source, UI_32 cmd, PCSTR sysInfo, UI_32 length, PCVOID data,
                                 EFrameworkunifiedMessagePriorties priority, UI_32 sessionid) {
  if (NULL == sysInfo) {
    return eFrameworkunifiedStatusInvldParam;
  }
  return mcSendInternal(hMessage, source, cmd, sysInfo, length, data, priority, sessionid, FALSE);
}

EFrameworkunifiedStatus McSendWithSysInfo(HANDLE hMessage, PCSTR source, UI_32 cmd, PCSTR sysInfo, UI_32 length, PCVOID data,
                             UI_32 sessionid) {
  if (NULL == sysInfo) {
    return eFrameworkunifiedStatusInvldParam;
  }
  return mcSendInternal(hMessage, source, cmd, sysInfo, length, data, eFrameworkunifiedMsgPrioNormal, sessionid, FALSE);
}

EFrameworkunifiedStatus McSend(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data) {
  return mcSendInternal(hMessage, source, cmd, NULL, length, data, eFrameworkunifiedMsgPrioNormal, 0, FALSE);
}

EFrameworkunifiedStatus McSendWithPriority(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data,
                              EFrameworkunifiedMessagePriorties priority,  UI_32 sessionid) {
  return mcSendInternal(hMessage, source, cmd, NULL, length, data, priority, sessionid, FALSE);
}

EFrameworkunifiedStatus McSendWithSession(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data, UI_32 sessionid) {
  return mcSendInternal(hMessage, source, cmd, NULL, length, data, eFrameworkunifiedMsgPrioNormal, sessionid, FALSE);
}

EFrameworkunifiedStatus McSendSyncResponse(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 seq_id, EFrameworkunifiedStatus ret_val, UI_32 length,
                              PCVOID data) {
  EFrameworkunifiedStatus eStatus;
  ResponseHeader *resHdr;
  UI_8 *res_data;

  if ((UINT_MAX - sizeof(ResponseHeader)) < length) {
    return eFrameworkunifiedStatusInvldBufSize;
  }

  // check if buffer is null
  if (data == NULL && length > 0) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  if ((res_data = calloc(length + sizeof(ResponseHeader), 1)) == NULL) {  // LCOV_EXCL_BR_LINE 5: calloc's error case
    // LCOV_EXCL_START 5: calloc's error case
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP
  }

  resHdr = (ResponseHeader *)res_data;
  resHdr->seq_id = seq_id;
  resHdr->ret_val = ret_val;
  resHdr->res_size = length;

  memcpy(res_data + sizeof(ResponseHeader), data, length);

  eStatus = mcSendInternal(hMessage, source, cmd, NULL, (UI_32)(length + sizeof(ResponseHeader)), res_data, eFrameworkunifiedMsgPrioNormal, 0,
                           TRUE);

exit:
  free(res_data);
  res_data = NULL;
  resHdr = NULL;

  return eStatus;
}

/*
 * Xorshift
 */
static inline unsigned int xor128(void) {
  unsigned int t;
  t = (mcSeed[0] ^ (mcSeed[0] << 11));
  mcSeed[0] = mcSeed[1];
  mcSeed[1] = mcSeed[2];
  mcSeed[2] = mcSeed[3];
  return (mcSeed[3] = (mcSeed[3] ^ (mcSeed[3] >> 19)) ^ (t ^ (t >> 8)));
}

static inline char *ultoa(unsigned long value, char *buf, int radix) {
  const static char c[] = "0123456789abcdef";
  char b[65];
  char *p = b + sizeof(b);

  *--p = '\0';
  do {
    *--p = c[value % (unsigned long)radix];
    value /= (unsigned long)radix;
  } while (value);
  if (buf == NULL) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "buf is NULL");
    return NULL;
  }
  strcpy(buf, p);

  return buf;
}

EFrameworkunifiedStatus McInvokeSync(HANDLE hMessage, PCSTR source, UI_32 cmd, UI_32 msgLength, PCVOID msgData, UI_32 sessionid,
                        HANDLE hRcvMessage, UI_32 responseLength, PVOID responseData, UI_32 *receivedLength) {
  EFrameworkunifiedStatus eStatus;
  TSysMsgSystemInfo sysInfo;
  UI_32 seq_id;
  ResponseHeader *resHdr;
  UI_8 *rcv_buf = NULL;
  CHAR resSource[MAX_NAME_SIZE_APP];
  int efd = -1;
  struct epoll_event events[2];
  struct epoll_event ev;
  int connect_sock = -1;
  SQhandle *sndHndl = (SQhandle *)hMessage;

  if (mqCheckValidHandle(hMessage) == FALSE || mqCheckValidHandle(hRcvMessage) == FALSE || (responseLength != 0 &&
                                                                                            responseData == NULL)) {
    eStatus = eFrameworkunifiedStatusInvldHandle;

    /**
     * @todo
     * The system performs rcv_buf free without NULL determination.
     */
    goto exit;
  }

  if (NULL == receivedLength) {
    eStatus = eFrameworkunifiedStatusInvldParam;
    goto exit;
  }
  if ((rcv_buf = malloc(MAX_QUEUE_MSG_SIZE)) == NULL) {  // LCOV_EXCL_BR_LINE 5: malloc's error case.
    // LCOV_EXCL_START 5: malloc's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP
  }

  if (mcSeed[0] == 0) {
    int fd;
    if ((fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC)) < 0) {
      eStatus = eFrameworkunifiedStatusFail;
      goto exit;
    }

    if (read(fd, &mcSeed, sizeof(mcSeed)) < 0) {
      close(fd);
      eStatus = eFrameworkunifiedStatusFail;
      goto exit;
    }
    close(fd);
  }
  seq_id = xor128();

  sysInfo[0] = '0';
  sysInfo[1] = 'x';
  ultoa(seq_id, &sysInfo[2], 16);

  Flush(hRcvMessage);

  eStatus = mcSendInternal(hMessage, source, cmd, sysInfo, msgLength, msgData, eFrameworkunifiedMsgPrioNormal, sessionid, TRUE);
  if (eStatus != eFrameworkunifiedStatusOK) {
    goto exit;
  }

  if ((connect_sock = mcConnectMonitor(sndHndl->q_name + 1, source)) < 0) {
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "connect:%s", strerror(errno));
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
  }

  if ((efd = epoll_create1(EPOLL_CLOEXEC)) < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_create1's error case.
    // LCOV_EXCL_START 5: epoll_create1's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_create1:%s", strerror(errno));
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP
  }
  ev.events = EPOLLIN;
  ev.data.fd = McGetQueueFD(hRcvMessage);
  if (epoll_ctl(efd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_ctl's error case.
    // LCOV_EXCL_START 5: epoll_ctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_ctl:%s", strerror(errno));
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP
  }
  ev.events = EPOLLRDHUP;
  ev.data.fd = connect_sock;
  if (epoll_ctl(efd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0) {  // LCOV_EXCL_BR_LINE 5: epoll_ctl's error case.
    // LCOV_EXCL_START 5: epoll_ctl's error case.
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_ctl:%s", strerror(errno));
    eStatus = eFrameworkunifiedStatusFail;
    goto exit;
    // LCOV_EXCL_STOP
  }

  while (1) {
    int nfds, n;

    nfds = epoll_wait(efd, events, 2, MC_INVOKE_SYNC_TIMEOUT);
    if (nfds == -1) {  // LCOV_EXCL_BR_LINE 5: epoll_wait's error case.
      // LCOV_EXCL_START 5: epoll_wait's error case.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      if (errno == EINTR) {
        continue;
      }
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_wait:%s", strerror(errno));
      eStatus = eFrameworkunifiedStatusFail;
      goto exit;
      // LCOV_EXCL_STOP
    } else if (nfds == 0) {  // LCOV_EXCL_BR_LINE 5: epoll_wait return value equal 0.
      // LCOV_EXCL_START 5: epoll_wait return value equal 0.
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_wait:time out");
      eStatus = eFrameworkunifiedStatusAccessError;
      goto exit;
      // LCOV_EXCL_STOP
    }

    for (n = 0; n < nfds; n++) {
      if (events[n].data.fd == connect_sock) {
        /*
         * connection fail
         */
        char connect_key[32];
        mcSetConnectKey(connect_key, sizeof(connect_key), sndHndl->q_name + 1, source);

        if (epoll_ctl(efd, EPOLL_CTL_DEL, connect_sock, events) < 0) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_ctl:%s", strerror(errno));
        }
        if (close(connect_sock) < 0) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "close:%s", strerror(errno));
        }
        frameworkunifiedUnregistResouce(FRAMEWORKUNIFIED_RES_ABNMLMON, connect_key);

        if ((connect_sock = mcConnectMonitor(sndHndl->q_name + 1, source)) < 0) {
          eStatus = eFrameworkunifiedStatusBadConnection;
          goto exit;
        }
        ev.events = EPOLLRDHUP;
        ev.data.fd = connect_sock;
        if (epoll_ctl(efd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0) {
          FRAMEWORKUNIFIEDLOG(ZONE_NS_ERR, __func__, "epoll_ctl:%s", strerror(errno));
          eStatus = eFrameworkunifiedStatusFail;
          goto exit;
        }
      } else {
        /*
         * response receive
         */
        if ((eStatus = McReceive(hRcvMessage, resSource, &cmd, MAX_QUEUE_MSG_SIZE, rcv_buf)) != eFrameworkunifiedStatusOK) {
          goto exit;
        }

        PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)rcv_buf;
        if (MC_EMSGT_IS_SHM(sysHdr->eType)) {
          UI_32 shmLength = GetLengthOfDataFromShared(sysHdr->memId);
          TMemID memId = sysHdr->memId;

          if (shmLength == 0) {
            eStatus = eFrameworkunifiedStatusErrOther;
            goto exit;
          }

          free(rcv_buf);
          if ((rcv_buf = calloc(shmLength, 1)) == NULL) {
            eStatus = eFrameworkunifiedStatusFail;
            goto exit;
          }

          if ((eStatus = GetDataFromShared(memId, rcv_buf, shmLength)) != eFrameworkunifiedStatusOK) {
            eStatus = eFrameworkunifiedStatusErrOther;
            goto exit;
          }

          DiscardDataFromShared(memId);
          resHdr = (ResponseHeader *)rcv_buf;
        } else {
          resHdr = (ResponseHeader *)(rcv_buf + sizeof(SSystemMsgHeader));
        }

        if (resHdr->seq_id == seq_id) {
          if (resHdr->res_size > 0 && responseLength > 0)
            memcpy(responseData, (const void *)((char *)resHdr + sizeof(ResponseHeader)),
                   resHdr->res_size > responseLength ? responseLength : resHdr->res_size);

          *receivedLength = resHdr->res_size;
          eStatus = resHdr->ret_val;
          goto exit;  // success
        }
      }
    }
  }

exit:
  free(rcv_buf);
  rcv_buf = NULL;
  resHdr = NULL;

  if (efd >= 0) {
    close(efd);
  }

  return eStatus;
}

EFrameworkunifiedStatus McCreateInvokerName(PCSTR source, UI_32 sessionid, PSTR invokerName, UI_32 size) {
  if (source == NULL || invokerName == NULL) {
    return eFrameworkunifiedStatusInvldParam;
  }

  // When the size of the synchronous communication message queue name storage area is 0.
  if (0 == size) {
    return eFrameworkunifiedStatusInvldParam;
  }
  snprintf(invokerName, size - 1, "%s_r0", source);
  return eFrameworkunifiedStatusOK;
}

EFrameworkunifiedStatus McForward(HANDLE hMessage, PCSTR source, UI_32 iCmd, TMemID USID) {
  // check if handle is valid
  if (mqCheckValidHandle(hMessage) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // check if the source is null
  if (source == NULL) {
    return eFrameworkunifiedStatusInvldQName;
  }

  // check the shared memory id
  if (USID == BAD_MEM_ID) {
    return eFrameworkunifiedStatusErrOther;
  }

  // check if the source length exceeds than expected
  if (strlen(source) >= MAX_QUEUE_NAME_SIZE) {
    return eFrameworkunifiedStatusInvldQName;
  }

  UI_8 sndBuf[ MAX_QUEUE_MSG_SIZE ] = {};
  SQhandle *sndHndl = (SQhandle *)hMessage;
  SSystemMsgHeader msgHdr = {};

  // check if the dest length exceeds than expected
  if (strlen(sndHndl->q_name) >= MAX_QUEUE_NAME_SIZE) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // compose header
  strlcpy(msgHdr.info.source, source, sizeof(msgHdr.info.source));
  msgHdr.info.length = GetLengthOfDataFromShared(USID);  // this is length of the messages data...
  msgHdr.info.command = iCmd;
  msgHdr.eType = (UI_32)(MC_EMSGT_VALID_TOKEN | MC_EMSGT_SHM_MASK);
  msgHdr.memId = USID;
  msgHdr.info.sessionid = 0;

  // copy to buffer
  UI_8 offset = sizeof(SSystemMsgHeader);
  // write header to send buffer
  memcpy(&sndBuf[0], &msgHdr, offset);

  return SendMessage(hMessage, MAX_QUEUE_MSG_SIZE, (PVOID)&sndBuf[0]);
}

PCSTR McGetMsgSrc(PVOID data) {
  // check if buffer is null
  if (data == NULL) {
    return NULL;  // The data is invalid return zero length.
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  return (PCSTR)sysHdr->info.source;
}

UI_32 mcGetMsgSsessionId(PVOID data) {
  // check if buffer is null
  if (data == NULL) {
    return 0;  // The data is invalid return zero.
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  return sysHdr->info.sessionid;
}

UI_32 McGetLength(PVOID data) {
  // check if buffer is null
  if (data == NULL) {
    return 0;  // The data is invalid return zero length.
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  return sysHdr->info.length;
}

PVOID McGetDataPointer(PVOID data) {
  if (data == NULL) {
    return NULL;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  int offset = sizeof(SSystemMsgHeader);

  if (MC_EMSGT_HAS_SYSINFO(sysHdr->eType)) {
    offset += (int)sizeof(TSysMsgSystemInfo);
  }

  return (PVOID)((long)data + offset);
}

EFrameworkunifiedStatus McGetSysInfoData(PVOID data, PVOID to) {
  // check if buffers are null
  if (data == NULL || to == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  if (MC_EMSGT_VALID((int)sysHdr->eType)) {
    if (MC_EMSGT_HAS_SYSINFO(sysHdr->eType)) {
      strlcpy(to, (char *)data + sizeof(SSystemMsgHeader), MAX_SYS_INFO_SIZE);
    } else {
      *(char *)to = '\0';
    }
    return eFrameworkunifiedStatusOK;
  }

  return eFrameworkunifiedStatusInvldHandle;
}

EFrameworkunifiedStatus McGetDataOfSize(PVOID data, PVOID to, UI_32 uiSize) {
  if (McGetLength(data) > uiSize) {
    return eFrameworkunifiedStatusInvldBufSize;
  }
  return GetData(data, to, FALSE);
}

EFrameworkunifiedStatus McGetDataOfSizeWithSMRetain(PVOID data, PVOID to, UI_32 uiSize) {
  if (McGetLength(data) > uiSize) {
    return eFrameworkunifiedStatusInvldBufSize;
  }
  return GetData(data, to, TRUE);
}

BOOL mcGetIsTypeOfSync(PVOID data) {
  if (data == NULL) {
    return FALSE;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;

  if (MC_EMSGT_VALID((int)sysHdr->eType)) {
    if (MC_EMSGT_IS_SYNC(sysHdr->eType)) {
      return TRUE;
    }
  }

  return FALSE;
}

EFrameworkunifiedStatus McClearData(PVOID data) {
  // check if buffers are null
  if (data == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;

  if (MC_EMSGT_VALID((int)sysHdr->eType)) {
    if (MC_EMSGT_IS_SHM(sysHdr->eType)) {
      return DiscardDataFromShared(sysHdr->memId);
    } else {
      return eFrameworkunifiedStatusOK;
    }
  }

  return eFrameworkunifiedStatusInvldParam;
}

EFrameworkunifiedStatus McClose(HANDLE hMessage) {
  EFrameworkunifiedStatus rtnStatus = eFrameworkunifiedStatusInvldHandle;

  if (NULL != hMessage) {
    EQType qType = GetQueueType(hMessage);

    if (eQTypeSender == qType) {
      rtnStatus = CloseSender(hMessage);
    } else if (eQTypeReveiver == qType) {
      rtnStatus = CloseReceiver(hMessage);
    }
  }

  return rtnStatus;
}


void McFlushReceiver(HANDLE hMessage) {
  if (hMessage != INVALID_HANDLE) {
    // proxy to the real function in msg queue
    Flush(hMessage);
  }
}

TMemID McGetDataUSID(PVOID pData) {
  // check if buffer is null
  if (pData == NULL) {
    return BAD_MEM_ID;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)pData;

  if (MC_EMSGT_VALID((int)sysHdr->eType)) {
    if (MC_EMSGT_IS_SHM(sysHdr->eType)) {
      return sysHdr->memId;
    } else {
      return BAD_MEM_ID;
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// GetData
////////////////////////////////////////////////////////////////////////////////////////////
EFrameworkunifiedStatus GetData(PVOID data, PVOID to, BOOL retain) {
  EFrameworkunifiedStatus rtnStatus = eFrameworkunifiedStatusInvldHandle;

  // check if buffers are null
  if (data == NULL || to == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  PSSystemMsgHeader sysHdr = (PSSystemMsgHeader)data;
  int offset = sizeof(SSystemMsgHeader);

  if (MC_EMSGT_VALID((int)sysHdr->eType)) {
    if (MC_EMSGT_HAS_SYSINFO(sysHdr->eType)) {
      offset += (int)sizeof(TSysMsgSystemInfo);
    }

    if (MC_EMSGT_IS_SHM(sysHdr->eType)) {
      // need to get the message from shared memory

      // get the length based on the memId
      UI_32 shmLength = GetLengthOfDataFromShared(sysHdr->memId);
      /*
       * todo
       * When a eFrameworkunifiedStatusInvldID error occurs in GetDataFromShared, the abnormal values are also returned in eFrameworkunifiedStatusErrOther.
       */
      if (0 != shmLength && eFrameworkunifiedStatusOK == GetDataFromShared(sysHdr->memId, to, shmLength)) {
        if (!retain) {
          // now that the data has been retrieved we may now discard it.
          McClearData(data);
        }
        rtnStatus = eFrameworkunifiedStatusOK;
      } else {
        rtnStatus = eFrameworkunifiedStatusErrOther;
      }
    } else {
      // memcpy the data to the buffer provided.
      memcpy(to, (PVOID)((long)data + offset), sysHdr->info.length);
      rtnStatus = eFrameworkunifiedStatusOK;
    }
  } else {
    rtnStatus = eFrameworkunifiedStatusInvldHndlType;
  }

  return rtnStatus;
}

PCSTR McGetQueueName(HANDLE hMessage) {
  PCSTR name = NULL;

  if (NULL != hMessage) {
    name = GetQueueName(hMessage);
  }

  return name;
}

int McGetQueueFD(HANDLE hMessage) {
  int fd = -1;

  if (NULL != hMessage) {
    fd = GetQueueFD(hMessage);
  }

  return fd;
}

EFrameworkunifiedStatus McTranslateError(int error) {
  return TranslateError(error);
}

EFrameworkunifiedStatus McZcSetParam(HANDLE handle, UI_32 cmd, UI_32 length) {
  if (handle == NULL) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  SQhandle *sndHndl = (SQhandle *)handle;
  SSystemMsgHeader *msgHdr = (SSystemMsgHeader *)(sndHndl->sendbuf);

  // QueueType checking
  EQType qType = GetQueueType(handle);
  if (eQTypeSender != qType) {
    return eFrameworkunifiedStatusInvldHndlType;
  }

  if (mqCheckValidHandle(handle) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }
  if (sndHndl->sendbuf == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }
  if (length > (MAX_QUEUE_MSG_SIZE - sizeof(SSystemMsgHeader))) {
    return eFrameworkunifiedStatusInvldBufSize;
  }

  msgHdr->info.command = cmd;
  msgHdr->info.length = length;
  return eFrameworkunifiedStatusOK;
}

PVOID McZcGetBuf(HANDLE handle) {
  if (handle == NULL) {
    return NULL;
  }

  SQhandle *sndHndl = (SQhandle *)handle;

  // QueueType checking
  EQType qType = GetQueueType(handle);
  if (eQTypeSender != qType) {
    return NULL;
  }

  if (mqCheckValidHandle(handle) == FALSE) {
    return NULL;
  }
  if (sndHndl->sendbuf == NULL) {
    return NULL;
  }
  return (char *)(sndHndl->sendbuf) + sizeof(SSystemMsgHeader);
}

EFrameworkunifiedStatus McZcSend(HANDLE hMessage) {
  if (hMessage == NULL) {
    return eFrameworkunifiedStatusInvldHandle;
  }

  // QueueType checking
  EQType qType = GetQueueType(hMessage);
  if (eQTypeSender != qType) {
    return eFrameworkunifiedStatusInvldHndlType;
  }

  SQhandle *sndHndl = (SQhandle *)hMessage;

  SSystemMsgHeader *msgHdr = (SSystemMsgHeader *)(sndHndl->sendbuf);

  if (mqCheckValidHandle(hMessage) == FALSE) {
    return eFrameworkunifiedStatusInvldHandle;
  }
  if (sndHndl->sendbuf == NULL) {
    return eFrameworkunifiedStatusInvldBuf;
  }

  return SendMessageWithPriority(hMessage, (UI_32)(msgHdr->info.length + sizeof(SSystemMsgHeader)), sndHndl->sendbuf,
                                 eFrameworkunifiedMsgPrioNormal);
}

HANDLE McZcOpenSender(PCSTR source) {
  HANDLE handle = openSenderZc(source);

  if (handle != NULL) {
    SQhandle *sndHndl = (SQhandle *)handle;
    SSystemMsgHeader *msgHdr = (SSystemMsgHeader *)(sndHndl->sendbuf);

    strlcpy(msgHdr->info.source, source, sizeof(msgHdr->info.source));
    msgHdr->info.command = 0;
    msgHdr->info.length = 0;
    msgHdr->info.sessionid = 0;
    msgHdr->eType = (UI_32)MC_EMSGT_VALID_TOKEN;
    msgHdr->memId = 0;
  }

  return handle;
}

EFrameworkunifiedStatus McZcClose(HANDLE handle) {
  return CloseSender(handle);
}
