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

#ifndef OTHERSERVICE_RPCUDP_H_  // NOLINT(build/header_guard)
#define OTHERSERVICE_RPCUDP_H_  // NOLINT(build/header_guard)

/** @addtogroup RPClib_in */
/** @{ */
/** Number of bytes of payload that can be stored in a UDP packet */
#define RPC_UDP_PAYLOAD RPC_MAX_API_ARG_TOTAL_SIZE

/** UDP packet header
 *
 * Packet type, source ID, sequence number, packet size, packet position
 */
#define RPC_PACKET_HEADER "%-2d %-5x %-8x %-4u %1d "
/** Number of bytes in the UDP packet header
 */
#define RPC_PACKET_HEADER_LEN (2+1 + 5+1 + 8+1 + 4+1 + 1+1)

/** Number of bytes to store in the UDP packet */
#define RPC_UDP_PACKET_SIZE (RPC_UDP_PAYLOAD + RPC_PACKET_HEADER_LEN)

#define RPC_COMMAND_LEN 3

/** Format string for UDP packet header analysis */
#define RPC_PACKET_HEADER_scanf "%d %x %x %d"

/** Header (API number) to be added to the API call packet */
#define RPC_APICALL_FORMAT "%-4d "
/** Number of bytes in the header of an API call packet */
#define RPC_APICALL_FORMAT_ARGS_START 5

#define RPC_MAX_RESPONSE_MESSAGE_SIZE 5

#define RPC_SEQ_NUM_START 1
#define RPC_SEQ_NUM_INVALID 0

/** Internal API call number for checking server existence */
#define RPC_API_NUM_RPC_ALIVE 0

/** Type of packet that the RPC library sends */
typedef enum {
  RPC_PACKET_APICALL = 0,/**< API call request */

  RPC_RESPONSE_APICALL,/**< Responding to API Call Requests */
#define RPC_RESPONSE_NONE 0x0000
#define RPC_RESPONSE_API_OK 0x0001
#define RPC_RESPONSE_API_BUSY 0x0002
#define RPC_RESPONSE_API_ERR 0x0004
#define RPC_RESPONSE_API_DEADLOCK 0x0005  /* Server deadlock */
#define RPC_RESPONSE_API_CERTIFY 0x0006    /* Authentication request */

  RPC_PACKET_APIRETURN,/**< Result of the API call */

  RPC_PACKET_NONE,
} RPC_packet_type;

/** Packet Location (Multi-Fragment Packet Support) */
typedef enum {
  RPC_PACKET_POS_ONEANDONLY = 0,/**< Complete with one packet */
} rpc_packet_position;

/** Transmitting buffer */
typedef struct {
  unsigned int bytes;
  const void *buf;
} rpc_send_buf;

/** Valid bytes in the sockaddr_un struct specified for send */
#define RPC_SOCKET_NAME_LEN 6 /**< Name: 5 characters + first NULL bytes */
#define RPC_SOCKET_ADDR_LEN (sizeof(short) + RPC_SOCKET_NAME_LEN)  // NOLINT (readability/nolint)

enum {
  RPC_SEND_TO_CLIENT,
  RPC_SEND_TO_SERVER,
  RPC_SEND_TO_SERVER_NO_RETRY,
};

struct RpcIdInfo;

int RpcSendUdp(struct RpcIdInfo *id, RPC_ID receiver, int direction,
     RPC_packet_type type, const void *mesg, unsigned int bytes);
int RpcSendUdp2(struct RpcIdInfo *id, RPC_ID receiver, int direction,
                  RPC_packet_type type, unsigned int num, rpc_send_buf *sendbuf);
RPC_Result RpcSendUdpResponse(struct RpcIdInfo *id, RPC_ID receiver,
         int direction,
         RPC_packet_type type,
         UINT32 seq_num, char *mesg, UINT32 bytes);

int RpcReadUdpPacket(const struct RpcIdInfo *id, UINT8 *buf);

RPC_Result RpcParsePacketHeader(const char *str, RPC_packet_type *command,
           RPC_ID_p id, UINT32 *seq_num, UINT32 *size);

RPC_Result RpcClientWaitResponse(struct RpcIdInfo *idinfo,
            UINT32 seq_num,
            UINT32 timeout_msec, UINT16 *response);
RPC_Result RpcClientWaitResult(struct RpcIdInfo *idinfo, RPC_ID srvr_id);

#if defined(RPC_USE_UNIX_AUTOBIND)
/* ===pathname of datagram sockets for API requests=== */
static inline void
RpcSetServerName(char *str, RPC_ID id) {
  *str = '\0';
  sprintf(str + 1, "S%04x", id);  // NOLINT (readability/nolint)
}

static inline void
RpcSetClientName(char *str, RPC_ID id) {
  *str = '\0';
  sprintf(str + 1, "%05x", id);  // NOLINT (readability/nolint)
}

static inline void
RpcGetClientName(const char *str, RPC_ID *id) {
  char buf[6];
  memcpy(buf, str + 1, 5);
  buf[5] = '\0';
  if (buf[1] == 'X') {
    *id = (RPC_ID)(strtoul(buf + 1, NULL, 16));
  } else {
    *id = (RPC_ID)(strtoul(buf, NULL, 16));
  }
}

/* ===pathname of stream-socket for authenticating=== */
static inline void
RpcSetServerSecureName(char *str, RPC_ID id) {
  *str = '\0';
  sprintf(str + 1, "secure_S%04x", id);  // NOLINT (readability/nolint)
}

static inline void
RpcSetClientSecureName(char *str, RPC_ID id) {
  *str = '\0';
  sprintf(str + 1, "secure_%05x", id);  // NOLINT (readability/nolint)
}

static inline void
RpcGetClientNameFromSock(const char *str, char *client_sock_name) {
  memcpy(client_sock_name, str + 8, 5);
  client_sock_name[5] = '\0';
}

#endif /* !AUTOBIND */

/** @} */
#endif  // OTHERSERVICE_RPCUDP_H_
