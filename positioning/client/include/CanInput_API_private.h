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

/******************************************************************************
 * File name        : CanInput_API_private.h
 * System name        : PastModel002
 * Sub System name    : CanInput_API library
 ******************************************************************************/
#ifndef POSITIONING_CLIENT_INCLUDE_CANINPUT_API_PRIVATE_H_
#define POSITIONING_CLIENT_INCLUDE_CANINPUT_API_PRIVATE_H_

/************************************************************************
 *            Include                                                   *
 ***********************************************************************/

/************************************************************************
*            Definition                                                 *
************************************************************************/
/*----------------------------------------------------------------------*
 *    Message Definition                                                *
 *----------------------------------------------------------------------*/
/* Message body size (byte) */
#define CANINPUT_MSGBUF_DSIZE    8    /* Maximum message body size                */

/************************************************************************
*            type Definition                                            *
************************************************************************/

/************************************************************************
*            struct Definition                                          *
************************************************************************/
/*****************************************************************************
 * TAG         : CANINPUT_MSG_INFO
 * ABSTRACT     : CanInput_API message structure
 * NOTE         : Message structure
 *****************************************************************************/
typedef struct {
    T_APIMSG_MSGBUF_HEADER    hdr;                            /* Message header */
    uint8_t                   data[CANINPUT_MSGBUF_DSIZE];    /* Message data (Localtime) */
} CANINPUT_MSG_INFO;

/************************************************************************
 *            Function prototypes                                       *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*[CanInput_API Private API]*/

#ifdef __cplusplus
}
#endif

#endif  // POSITIONING_CLIENT_INCLUDE_CANINPUT_API_PRIVATE_H_
