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
 * @file rpc_udp.c
 * @brief RPC Library Internal Implementation--UDP Communication
 *
 */
/** @addtogroup RPClib_in */
/** @{ */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> // for usleep

#include <fcntl.h>
#include <sys/un.h>

#include <sys/inotify.h>

#include <other_service/rpc.h>
#include "rpc_internal.h"

static /*inline*/ UINT32
RpcGetSequenceNumber(RpcThreadInfo *th) {
  RPC_THREAD_MUTEX_LOCK(th);
  UINT32 ret = th->sequence_number;
  (th->sequence_number)++;
  if (th->sequence_number == RPC_SEQ_NUM_INVALID) {  // LCOV_EXCL_BR_LINE 200: overflow check, but test time is too long
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    th->sequence_number = RPC_SEQ_NUM_START;  // LCOV_EXCL_LINE 200: overflow check, but test time is too long
  }
  RPC_THREAD_MUTEX_UNLOCK(th);
  return ret;
}

/**
 */
int
RpcReadUdpPacket(const RpcIdInfo *id, UINT8 *buf) {
  struct sockaddr_un sa;
  socklen_t sa_len = sizeof(sa);
  // sa passed to recvfrom does not require initialization

  for(;;) {

    ssize_t ret = recvfrom(RPC_my_sock(id), buf, RPC_UDP_PACKET_SIZE,
       0, (struct sockaddr *)&sa, &sa_len);

    if (ret < 0) {
      if (errno == EAGAIN) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc recvfrom
        //RPC_LOG_PERROR("recvfrom port %d", RPC_port(id));
        return 0;
      } else if (errno == EINTR) {  // LCOV_EXCL_START 5: fail safe for libc recvfrom
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        continue;
      } else {
        RPC_LOG_PERROR("recvfrom port %d", RPC_port(id));
        return -1;
      }  // LCOV_EXCL_STOP
    } else if (ret == 0) {
      RPC_LOG_STATE("*** recvfrom ret 0");
      return 0;
    } else {
      return (int)ret;
    }
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  }  // LCOV_EXCL_LINE 10: final line
}

/** Number of sendto retries */
#define RPC_SENDTO_RETRY 5

RUNS_IN_CALLERS_THREAD
RUNS_IN_READING_THREAD
/**
 */
static int
RpcSendUdpPacket(RpcIdInfo *id,
        struct sockaddr_un *to, int do_retry,
        void *mesg, unsigned int bytes) {
  int nretry = 0;
  ssize_t ret;
  int myerr;
 retry:
  ret = sendto(RPC_my_sock(id), mesg, bytes, 0,
         (struct sockaddr *)to, RPC_SOCKET_ADDR_LEN);
  myerr = errno;

  if (ret < 0) {
    RPC_LOG_STATE("*** sendto %s ***", to->sun_path + 1);
    if (myerr == EAGAIN  || (do_retry && myerr==ECONNREFUSED && ++nretry <= RPC_SENDTO_RETRY)) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
      usleep(100000);
#if defined(RPC_USE_UNIX_AUTOBIND)
      RPC_LOG_DEBUG("*** sendto %s ***", to->sun_path + 1);
#else  /* AUTOBIND */
      RPC_LOG_DEBUG("**************** sendto retry *********************");
#endif  /* !AUTOBIND */
      goto retry;
    }
    errno = myerr;
#if defined(RPC_USE_UNIX_AUTOBIND)
    if (do_retry) {
      RPC_LOG_PERROR("sendto %s", to->sun_path + 1);
    }
#endif  /* !AUTOBIND */
    return -1;
  } else if ((unsigned int)ret != bytes) {
    RPC_LOG_STATE("can't send all");
    return -1;
  }
  return 0;
}

RUNS_IN_CALLERS_THREAD
RUNS_IN_READING_THREAD
/**
 */
int
RpcSendUdp(RpcIdInfo *id, const RPC_ID receiver, int direction, RPC_packet_type type, const void *mesg, unsigned int bytes) {  // LCOV_EXCL_START 8: dead code
  AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
  rpc_send_buf sendbuf;
  sendbuf.buf = mesg;
  sendbuf.bytes = bytes;

  return RpcSendUdp2(id, receiver, direction, type, 1, &sendbuf);
}  // LCOV_EXCL_STOP
RUNS_IN_CALLERS_THREAD
RUNS_IN_READING_THREAD
/** UDP packet transmission process
 *
 * UDP packet format
 * @verbatim
+--------------+--------------+
|*1|*2|*3|*4|*5|   PAYLOAD    |
+--------------+--------------+
@endverbatim
 *
 * *1 to *5 indicate the header. @see RPC_PACKET_HEADER
 *
 * - *1: [@%-2d] Type of packet
 *   - @see RPC_packet_type
 * - *2: [@%-5x] Source RPC_ID
 * - *3: [@%-8x] Sequence number
 *   - Set the value incremented from 1. This value must be unique for each source thread.
 *   - In the case of response packets, this field contains the sequence number of the packet from which the response was received.
 * - *4: [@%-4u] Size of send data
 *   - In the case of sending data consists of single packet in PAYLOAD, this field contains number of the packet.
 *   - For a multipacket, the first packet contains the sum of the PAYLOAD of all packets,
 *     the last packet contains the PAYLOAD bytes of the packet.
 *     All other intermediate packets are filled with 0.(With up to two packets in the current constraint,
 *     there are no intermediate packets.)
 * - *5: [@%1d]  Position of packet
 *   - @see rpc_packet_position
 */
int
RpcSendUdp2(struct RpcIdInfo *id, RPC_ID receiver, int direction,
        RPC_packet_type type, unsigned int num, rpc_send_buf *sendbuf) {
  unsigned char buf[RPC_UDP_PACKET_SIZE];
  UINT32 seq_num = RpcGetSequenceNumber(id->thread_info);
  unsigned int bytes = 0;
  rpc_send_buf *sendbufp = sendbuf;
  int i, do_retry = 1;
  for(i = 0 ; i < num ; i++) {
    bytes += sendbufp->bytes;
    sendbufp++;
  }
  rpc_assert(bytes <= RPC_UDP_PAYLOAD);  // LCOV_EXCL_BR_LINE 6: double check

  struct sockaddr_un to;
  memset(&to, 0, sizeof(to));
  to.sun_family = AF_UNIX;
#if defined(RPC_USE_UNIX_AUTOBIND)
  if (direction != RPC_SEND_TO_CLIENT) {
    RpcSetServerName(to.sun_path, receiver);
    if (direction == RPC_SEND_TO_SERVER_NO_RETRY) {
      do_retry = 0;
    }
  } else {
    RpcSetClientName(to.sun_path, receiver);
  }
#else  /* !AUTOBIND */
  rpc_set_socket_name(to.sun_path, rpc_get_port(receiver));
#endif  /* !AUTOBIND */

  sprintf((char *)buf, RPC_PACKET_HEADER,
    (int)type, RPC_my_id(id), seq_num, bytes,
    RPC_PACKET_POS_ONEANDONLY);

  unsigned char *bufp = buf + RPC_PACKET_HEADER_LEN;
  for(i = 0 ; i < num ; i++) {
    memcpy(bufp, sendbuf->buf, sendbuf->bytes);
    bufp += sendbuf->bytes;
    sendbuf++;
  }
  if (RpcSendUdpPacket(id, &to, do_retry,
        buf, RPC_PACKET_HEADER_LEN + bytes) < 0) {
    return -1;
  }
  return (int)seq_num;
}

RUNS_IN_READING_THREAD
/**
 */
RPC_Result
RpcSendUdpResponse(RpcIdInfo *id, const RPC_ID receiver, int direction,
          RPC_packet_type type, UINT32 seq_num,
          char *mesg, UINT32 bytes) {
  rpc_assert(bytes <= RPC_MAX_RESPONSE_MESSAGE_SIZE);  // LCOV_EXCL_BR_LINE 6: double check
  char buf[RPC_PACKET_HEADER_LEN + RPC_MAX_RESPONSE_MESSAGE_SIZE];
  sprintf(buf, RPC_PACKET_HEADER,
    (int)type, RPC_my_id(id), seq_num, bytes,
    (int)RPC_PACKET_POS_ONEANDONLY);
  memcpy(buf + RPC_PACKET_HEADER_LEN, mesg, bytes);

  struct sockaddr_un sa;
  memset(&sa, 0, sizeof(sa));
  sa.sun_family = AF_UNIX;
#if defined(RPC_USE_UNIX_AUTOBIND)
  if (direction != RPC_SEND_TO_CLIENT) {
    RpcSetServerName(sa.sun_path, receiver);
  } else {
    RpcSetClientName(sa.sun_path, receiver);
  }
#else  /* !AUTOBIND */
  rpc_set_socket_name(sa.sun_path, rpc_get_port(receiver));
#endif  /* !AUTOBIND */
  return RpcSendUdpPacket(id, &sa, 0, buf, RPC_PACKET_HEADER_LEN + bytes);
}

/**
 */
RPC_Result
RpcParsePacketHeader(const char *str, RPC_packet_type *command,
      RPC_ID_p id, UINT32 *seq_num, UINT32 *size) {
  // LCOV_EXCL_BR_START 6: double check
  rpc_assert(str != NULL && command != NULL && id != NULL
       && seq_num != NULL && size != NULL);
  // LCOV_EXCL_BR_STOP

  if (sscanf(str, RPC_PACKET_HEADER_scanf, (int *)command, id, seq_num, size) != 4) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
    *command = RPC_PACKET_NONE;  // LCOV_EXCL_START 5: fail safe for libc sscanf
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    BUG_ASSERT(0, "Parsing packet failed");
    // LCOV_EXCL_STOP
    return RPC_ERR_Fatal;
  }
  return RPC_OK;
}

#include <sys/time.h>

RPC_Result
RpcClientWaitResponse(RpcIdInfo *idinfo, UINT32 seq_num,
       UINT32 timeout_msec, UINT16 *response) {
  unsigned char readbuf[RPC_UDP_PACKET_SIZE];
  fd_set fds;
  int fd = idinfo->sock;

  struct timeval timeout;
  timeout.tv_sec = (__time_t)(timeout_msec / 1000);
  timeout.tv_usec = (__suseconds_t)((timeout_msec % 1000) * 1000);

  *response = RPC_RESPONSE_NONE;

  for(;;) {
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    int sret = select(fd + 1, &fds, NULL, NULL, &timeout);
    if (sret < 0 && errno == EINTR) {  /* signal interrupt */  // LCOV_EXCL_START 5: fail safe for libc select
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      continue;
      // LCOV_EXCL_STOP
    } else if (sret == 0) {  /* timeout */
      RPC_LOG_ERR("server response timeout");
      return RPC_ERR_No_Response;
    } else if (sret > 0 && FD_ISSET(fd, &fds)) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc select
      RPC_ID sender;
      UINT32 seq;
      UINT32 size;
      RPC_packet_type command;
      int readret = RpcReadUdpPacket(idinfo, readbuf);
      if (readret <= 0) {  // LCOV_EXCL_START 5: fail safe for libc recvfrom
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        /* debug code to see socket status */
        /* why recvfrom() returned 0 ? */
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN|POLLHUP|POLLERR;
        sret = poll(&pfd, 1, 0/* timeout 0 */);
        RPC_LOG_STATE("** poll revents=%x", pfd.revents);
        return RPC_ERR_Fatal;
      }  // LCOV_EXCL_STOP
      if (RpcParsePacketHeader((const char *)readbuf, &command, &sender, &seq, &size) != RPC_OK) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
        return RPC_ERR_Fatal;
      }
      unsigned char c;
      if (seq == seq_num) {
        switch(command) {
          case RPC_RESPONSE_APICALL:
            c = readbuf[RPC_PACKET_HEADER_LEN];
            switch(c) {
              case 'O':
                *response = RPC_RESPONSE_API_OK;
                goto exit_loop_ok;
                break;
              case 'B':
                *response = RPC_RESPONSE_API_BUSY;
                goto exit_loop_ok;
                break;
              case 'E':
                *response = RPC_RESPONSE_API_ERR;
                goto exit_loop_ok;
                break;
              case 'D':
                *response = RPC_RESPONSE_API_DEADLOCK;
                goto exit_loop_ok;
                break;
              case 'C':
                *response = RPC_RESPONSE_API_CERTIFY;
                goto exit_loop_ok;
                break;
              default:
                AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
                BUG_ASSERT(0, "illegal response\n");  // LCOV_EXCL_LINE 15: marco defined in rpc_internal.h
                return RPC_ERR_Fatal;
                break;
            }
            break;

          default:
            RPC_LOG_STATE("Unknown packet command=%d", command);
            return RPC_ERR_Fatal;
            break;
        }
      } else {  /* sequence number mismatch == response to other request */
        RPC_LOG_DEBUG("unwanted response received(delayed response?)");
        continue;
      }
    } else {  /* poll error */
      RPC_LOG_PERROR("select in wait response");
      return RPC_ERR_Fatal;
    }
  }
  exit_loop_ok:
  return RPC_OK;
}

RPC_Result
RpcClientWaitResult(RpcIdInfo *idinfo, RPC_ID srvr_id) {
  unsigned char readbuf[RPC_UDP_PACKET_SIZE];
  fd_set fds;
  int fd = idinfo->sock;
  int inotify_fd = RPC_clnt_inotify_fd(idinfo);
  int maxfd;
  RPC_Result result = RPC_OK;

  for(;;) {
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    FD_SET(inotify_fd, &fds);

    /* Determine the maximum value of fd to wait */
    if (fd > inotify_fd) {
      maxfd = fd;
    } else {
      maxfd = inotify_fd;
    }

    int sret = select(maxfd + 1, &fds, NULL, NULL, NULL);
    if (sret < 0 && errno == EINTR) {  /* signal interrupt */  // LCOV_EXCL_START 5: fail safe for libc select
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      continue;
      // LCOV_EXCL_STOP
    }  else if (sret > 0 && FD_ISSET(fd, &fds)) {  /* success */  // LCOV_EXCL_BR_LINE 5: fail safe for libc select
      RPC_ID sender;
      UINT32 seq;
      UINT32 size;
      RPC_packet_type command;
      int readret = RpcReadUdpPacket(idinfo, readbuf);
      if (readret <= 0) {  // LCOV_EXCL_START 5: fail safe for libc recvfrom
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        rpc_assert(readret >= 0);
        result = RPC_ERR_Fatal;
        goto exit_loop_ok;
      }  // LCOV_EXCL_STOP
      if (RpcParsePacketHeader((const char *)readbuf, &command, &sender, &seq, &size) != RPC_OK) {  // LCOV_EXCL_BR_LINE 11: Unexpected branch // NOLINT(readability/nolint)
        // LCOV_EXCL_START 5: fail safe for libc sscanf
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        result = RPC_ERR_Fatal;
        goto exit_loop_ok;
      }  // LCOV_EXCL_STOP
      switch(command) {
        case RPC_PACKET_APIRETURN:
          result = RpcSetAPIcallReturn(idinfo,
              (const char *)(readbuf + RPC_PACKET_HEADER_LEN),
              size);
          if(result == RPC_OK) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc malloc
            if (sscanf((const char *)(readbuf + RPC_PACKET_HEADER_LEN), "%08x ", (unsigned int *)&result) != 1) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc sscanf
              AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
              BUG_ASSERT(0, "Parsing packet failed");  // LCOV_EXCL_LINE 15: marco defined in rpc_internal.h
              return RPC_ERR_Fatal;
            }
          }
          goto exit_loop_ok;
        default:
          RPC_LOG_STATE("unwanted packet received while waiting for API return");
          continue;
          break;
      }
    } else if (sret > 0 && FD_ISSET(inotify_fd, &fds)) {  /* server process is terminate. */
      UINT32 read_len = 0;
      int length = 0;
      char *buffer;
      buffer = (char *)rpc_malloc(BUF_LEN);
      if (NULL == buffer) {  // LCOV_EXCL_START 5: fail safe for libc malloc
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        RPC_LOG_ERR("rpc_malloc() ERROR.");
      }  // LCOV_EXCL_STOP

      if((length = (int)read( inotify_fd, buffer, BUF_LEN )) < 0 ) {  // LCOV_EXCL_START 5: fail safe for libc read
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        RPC_LOG_PERROR("inotify read() ERROR.");
      } // LCOV_EXCL_STOP
      while (read_len < length) {
        struct inotify_event *event = ( struct inotify_event * )&buffer[read_len];

        if (event->mask & IN_DELETE_SELF) {  /* Terminating a Server Process */
          if (RPC_ERR_Server_Finish == RpcDeleteSrvrPid(idinfo, srvr_id, event->wd)) {
            RPC_LOG_PERROR("server process is terminate. : srvr_ID = %x", srvr_id);
            result = RPC_ERR_Fatal;
          }
        }
        read_len += (UINT32)(EVENT_SIZE + event->len);  /* Size of the inotify_event structure */
      }
      rpc_free(buffer);
      if (RPC_OK != result) {
        goto exit_loop_ok;
      }

    } else {  /* poll error */  // LCOV_EXCL_START 5: fail safe for libc select
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      RPC_LOG_PERROR("select in wait result");
      result = RPC_ERR_Fatal;
      goto exit_loop_ok;
      // LCOV_EXCL_STOP
    }
  }
exit_loop_ok:

  return result;
}

/** @} */
