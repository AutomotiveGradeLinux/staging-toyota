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

#ifndef POSITIONING_SERVER_INCLUDE_COMMON_COM_COM_MESSAGE_HEADER_H_
#define POSITIONING_SERVER_INCLUDE_COMMON_COM_COM_MESSAGE_HEADER_H_

/**
 * @file com_message_header.h
 */


#ifndef _POS_MESSAGE_HEADER_

#include <sys/types.h>
#include <native_service/frameworkunified_types.h>

/**
 * @struct T_APIMSG_HEADER
 * \~english _CWORD64_API message header structure
 */
typedef struct {
    uint16_t replyid;          //!< \~english message send result notify dest ID
    uint16_t sndpno;           //!< \~english message send source process No
    uint16_t respno;           //!< \~english message response process No
    uint16_t cid;              //!< \~english message command ID
    uint16_t msgbodysize;      //!< \~english message body size
    uint8_t  rid;              //!< \~english message resourceID */
    uint8_t  reserve;          //!< \~english reserve
    uint8_t  filler[2];        //!< \~english filter
} T_APIMSG_HEADER;

/**
 * @struct T_APIMSG_MSGBUF_HEADER
 * \~english _CWORD64_API message buffer header structure
 */
typedef struct {
    uint32_t       signo;
    //!< \~english signal No, _pb_SndMsg to set 0
    T_APIMSG_HEADER hdr;   //!< \~english message header
} T_APIMSG_MSGBUF_HEADER;

/**
 * @struct _CWORD64_MSG_LOG_HDR
 * \~english _CWORD64_API message log header structure
 */
typedef struct {
    uint32_t    kick_time;    //!< \~english kick time
    uint32_t    sr_cid;       //!< \~english message command ID
    uint32_t    pno;         //!< \~english process No
    uint32_t    data_bytes;   //!< \~english data size
} _CWORD64_MSG_LOG_HDR;

#endif

#endif  // POSITIONING_SERVER_INCLUDE_COMMON_COM_COM_MESSAGE_HEADER_H_ */
