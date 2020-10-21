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

#ifndef COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COM_MESSAGE_HEADER_H_
#define COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COM_MESSAGE_HEADER_H_

/**
 * @file com_message_header.h
 * @~english
 * @brief Defination of structure for message header
 */

#include <sys/types.h>

/****************************************************************************
 * TAG      : typedef T_APIMSG_HEADER
 * ABSTRACT : _CWORD64_API message header structure
 * NOTE     :
 ****************************************************************************/
/*!
 * @~english
 * @brief Structure for message header
 */
typedef struct {
  uint16_t replyid;      //!< \~english Send Result ID
  uint16_t sndpno;       //!< \~english Send process No
  uint16_t respno;       //!< \~english Response No
  uint16_t cid;          //!< \~english Command ID
  uint16_t msgbodysize;  //!< \~english Message body size
  uint8_t rid;           //!< \~english Resource ID
  uint8_t reserve;       //!< \~english Reserve
  uint8_t filler[2];     //!< \~english filler
} T_APIMSG_HEADER_COMM;

/*!
 * @~english
 * @brief Structure for buffer message header
 */
typedef struct {
  uint32_t signo;            //!< \~english Signal No
  T_APIMSG_HEADER_COMM hdr;  //!< \~english Message Header
} T_APIMSG_MSGBUF_HEADER_COMM;

/*!
 * @~english  @brief Check for assert do not overflow buffer when Re-arrangement was cast to another structure
 */
#define assert_static(e)                \
  do {                                  \
    enum { assert_static__ = 1 / (e) }; \
  } while (0)

#endif  // COMMUNICATION_SUBCPU_CLIENT_INCLUDE_PERIPHERAL_SERVICE_COM_MESSAGE_HEADER_H_
