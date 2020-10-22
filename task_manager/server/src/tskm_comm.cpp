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

#ifndef _GNU_SOURCE
#warning "_GNU_SOURCE not defined, so define here"
#define _GNU_SOURCE
#endif
#include "tskm_comm.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <native_service/cl_monitor.h>

#include "tskm_debug.h"
#include "tskm_util.h"


/**********************************************************
 * Internal functions
 **********************************************************/
TSKM_STATIC TSKM_SRV_CONNENT_t*
addConnFd(TSKM_SRV_CONNENT_LIST_t* list, TSKM_SRV_CONNENT_t* conn) {
  TSKM_SRV_CONNENT_t* retConn;
  if (list->num >= TSKM_COMM_CONNECT_MAX) {
    TSKM_ASSERT(0)
    return NULL;
  }

  retConn = &list->conn[list->num];
  list->conn[list->num] = *conn;
  list->num++;
  return retConn;
}

TSKM_STATIC void delConnFd(TSKM_SRV_CONNENT_LIST_t* list,
                           TSKM_SRV_CONNENT_t* conn) {
  uint32_t ii;
  TSKM_BOOL_t isFind = TSKM_FALSE;
  for (ii = 0; ii < list->num; ii++) {
    if (!isFind && list->conn[ii].connFd == conn->connFd) {
      isFind = TSKM_TRUE;
      list->num--;
    }

    if (isFind && (ii < list->num)) {
      list->conn[ii] = list->conn[ii + 1];
    }
  }
  TSKM_ASSERT(isFind);
}

/**********************************************************
 * Public functions
 **********************************************************/
TSKM_ERR_t tskm_srvSockCreate(const char *sockName,
                              TSKM_SRV_SOCK_CTX_t* p_sock) {
  int fd = -1;
  int ret;
  int sockRet;
  int enable = 1;
  int listenNum = TSKM_COMM_CONNECT_MAX;

  struct sockaddr_un unix_addr = { };

  fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (fd < 0) {  // LCOV_EXCL_BR_LINE 5: For processing initializing process
    // LCOV_EXCL_START 5: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (0 == access(sockName, F_OK)) {  // LCOV_EXCL_BR_LINE 5: For processing initializing process
    TSKM_ASSERT(0);
    unlink(sockName);
  }

  unix_addr.sun_family = AF_UNIX;
  strncpy(unix_addr.sun_path, sockName, sizeof(unix_addr.sun_path) - 1);

  sockRet = bind(fd, (struct sockaddr *) &unix_addr, sizeof(unix_addr));
  if (sockRet != 0) {  // LCOV_EXCL_BR_LINE 5: For processing initializing process
    // LCOV_EXCL_START 5: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  ret = chmod(sockName, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
  if (ret != 0) {  // LCOV_EXCL_BR_LINE 5: For processing initializing process
    // LCOV_EXCL_START 5: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  sockRet = setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &enable, sizeof(enable));
  if (sockRet != 0) {  // LCOV_EXCL_BR_LINE 5:For processing initializing process
    // LCOV_EXCL_START 5: For processing initializing process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  sockRet = listen(fd, listenNum);
  if (sockRet != 0) {  // LCOV_EXCL_BR_LINE 5: For process initialization processing
    // LCOV_EXCL_START 5: For process initialization processing
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  memset(p_sock, 0, sizeof(*p_sock));
  p_sock->sockFd = fd;
  p_sock->sockName = sockName;

  return TSKM_E_OK;
  // LCOV_EXCL_START 5: For process initialization processing
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (fd != -1) {
    close(fd);
  }
  return TSKM_E_NG;
  // LCOV_EXCL_STOP
}

TSKM_SRV_CONNENT_t*
tskm_srvSockConnect(TSKM_SRV_SOCK_CTX_t* p_sock) {
  int fd = -1;
  socklen_t len;
  int sockRet;
  TSKM_SRV_CONNENT_t conn;
  TSKM_SRV_CONNENT_t *retConn;
  struct ucred credent;
  struct sockaddr_un unix_addr;

  memset(&conn, 0, sizeof(conn));
  conn.connFd = -1;

  len = sizeof(unix_addr);
  fd = accept(p_sock->sockFd, (struct sockaddr*) &unix_addr,
              (socklen_t *) &len);  // NOLINT (readability/casting)
  if (fd < 0) {  // LCOV_EXCL_BR_LINE 5: Accept's Error-Handling Process
    // LCOV_EXCL_START 5: Accept's Error-Handling Process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  conn.connFd = fd;

  len = sizeof(credent);
  sockRet = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &credent, &len);
  if (sockRet != 0) {  // LCOV_EXCL_BR_LINE 5: Getsockopt's Error-Handling Process
    // LCOV_EXCL_START 5: Getsockopt's Error-Handling Process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  conn.pid = credent.pid;

  retConn = addConnFd(&p_sock->connList, &conn);
  if (retConn == NULL) {  // LCOV_EXCL_BR_LINE 5: Connect's Error-Handling Process
    // LCOV_EXCL_START 5: Connect's Error-Handling Process
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  return retConn;
  // LCOV_EXCL_START 5: Error-Handling Process
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (conn.connFd != -1) {
    tskm_sockDestory(conn.connFd);
  }
  return NULL;
  // LCOV_EXCL_STOP
}

void tskm_srvSockDisconnect(TSKM_SRV_SOCK_CTX_t* p_sock,
                            TSKM_SRV_CONNENT_t *p_conn) {
  int fd = p_conn->connFd;
  delConnFd(&p_sock->connList, p_conn);
  tskm_sockDestory(fd);
}

void tskm_srvSockDestory(TSKM_SRV_SOCK_CTX_t* p_sock) {  // LCOV_EXCL_START 6: Because the condition cannot be set
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (p_sock->sockFd) {
    tskm_sockDestory(p_sock->sockFd);
    p_sock->sockFd = 0;

    TSKM_ASSERT_ERRNO(0 == unlink(p_sock->sockName));
  }
}
// LCOV_EXCL_STOP
int tskm_cliSockConnect(const char* sockName) {
  int fd = -1;
  int sockRet;
  struct sockaddr_un unix_addr = { };

  /* Create socket */
  fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (fd < 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  unix_addr.sun_family = AF_UNIX;
  strncpy(unix_addr.sun_path, sockName, sizeof(unix_addr.sun_path) - 1);
  sockRet = connect(fd, (struct sockaddr*) &unix_addr, sizeof(unix_addr));
  if (sockRet < 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  return fd;
  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  if (fd != -1) {
    TSKM_ASSERT_ERRNO(0 == close(fd));
  }
  return -1;
  // LCOV_EXCL_STOP
}

int tskm_sockRcv(int fd, TSKM_EVENT_INFO_t* p_ev) {
  int ret;
  TSKM_EVENT_INFO_t ev;
  ret = static_cast<int>(recv(fd, &ev, sizeof(ev), 0));

  // LCOV_EXCL_BR_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  if (ret > (int) sizeof(ev)) {  // NOLINT (readability/casting)
    // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }

  if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
    // LCOV_EXCL_STOP
  }
  if (ret > 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    TSKM_PRINTF(TSKM_LOG_MSG, "recv:%s from:%d ret:%d",
                tskm_convEvent2Str(ev.event), ev.fromPid, ret);
  }

  *p_ev = ev;

  if (p_ev->hasExtend && (0 != p_ev->extendSize)) {
    TSKM_PRINTF(TSKM_LOG_MSG, "rcv:ex(%d) ", p_ev->extendSize);

    p_ev->extendPrm = malloc(p_ev->extendSize);
    if (!p_ev->extendPrm) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }

    ret = static_cast<int>(recv(fd, p_ev->extendPrm, p_ev->extendSize, 0));

    // LCOV_EXCL_BR_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
    if (ret > (int) p_ev->extendSize) {  // NOLINT (readability/casting)
      // LCOV_EXCL_BR_STOP
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT_ERRNO(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }

    if (ret < 0) {  // LCOV_EXCL_BR_LINE 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      TSKM_ASSERT_ERRNO(0);
      goto ERROR;
      // LCOV_EXCL_STOP
    }
  }

  return ret;

  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return -1;
  // LCOV_EXCL_STOP
}

int tskm_sockSend(int fd, TSKM_EVENT_INFO_t* p_ev) {
  int ret;
  p_ev->fromPid = getpid();
  TSKM_PRINTF(TSKM_LOG_MSG, "send:%s ", tskm_convEvent2Str(p_ev->event));
  ret = static_cast<int>(send(fd, p_ev, sizeof(*p_ev), MSG_NOSIGNAL));
  if (ret != sizeof(*p_ev)) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }
  // Because it is entered only when called from a debugging function (pri_sendDebugDumpRes)
  if (p_ev->hasExtend && p_ev->extendPrm && (0 != p_ev->extendSize)) {
    TSKM_PRINTF(TSKM_LOG_MSG, "send:ex(%d) ", p_ev->extendSize);

    ret = static_cast<int>(send(fd, p_ev->extendPrm, p_ev->extendSize, MSG_NOSIGNAL));
    if (ret != (int)p_ev->extendSize) {  // NOLINT (readability/casting)
      TSKM_ASSERT_ERRNO(0);
      goto ERROR;
    }
  }

  return ret;

  ERROR: return -1;
}

void tskm_sockDestory(int fd) {
  TSKM_ASSERT_ERRNO(0 == shutdown(fd, SHUT_RDWR));
  TSKM_ASSERT_ERRNO(0 == close(fd));
}

/******************************************************************
 *        Initializing (Process)
 ******************************************************************/
int tskm_comm_procInit(void) {
  int ret;

  ret = CL_MonitorInit(CL_MONITOR_INIT_USER);

  if (ret != 0) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  return 0;

  // LCOV_EXCL_START 5: Checked in Death testing, but it is not reflected in the coverage and excluded
  ERROR:
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  return -1;
  // LCOV_EXCL_STOP
}

/*********************************************************
 *        Service Error Monitoring Status Setting
 *********************************************************/
int tskm_comm_setSvcWatchState(uint32_t id, BOOL bIsRun, uint32_t timeout) {
  int ret = 0;

  CL_MonitorState_t state = CL_MONITOR_STATE_SLEEP;

  if (bIsRun) {
    state = CL_MONITOR_STATE_RUN;
  }

  ret = CL_MonitorSetEntry(CL_MONITOR_TYPE_GENERIC, id, state, timeout, 0);
  if (0 != ret) {
    TSKM_ASSERT_ERRNO(0);
    goto ERROR;
  }

  return 0;
  ERROR: return -1;
}  // LCOV_EXCL_BR_LINE 10: Final line

