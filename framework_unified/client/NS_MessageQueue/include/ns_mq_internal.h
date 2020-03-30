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

///////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSMessageQueue
/// \brief    API Header for Logger.
///
/// Declares the external APIs to Logger.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_INTERNAL_H_
#define FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_INTERNAL_H_

/////////////////////////////////////////
#include <pthread.h>
/////////////////////////////////////////


/// \brief Struct that defines internal handle object
typedef struct tQHandle {
  UI_32   check_code;
  SI_32   fd;
  PSTR    q_name;
  EQType    q_type;
  void    *sendbuf;

  /////////////////////////////////////////
  pthread_t threadid;  // thread id of child thread (may be NULL)
  /////////////////////////////////////////
} SQhandle;

#define MQ_CHECK_CODE (0xC0D1F1ED)

static inline BOOL mqCheckValidHandle(HANDLE hMessage) {
  if (INVALID_HANDLE != hMessage) {
    SQhandle *sndHndl = (SQhandle *)hMessage;  // NOLINT  (readability/nolint)
    if (sndHndl->check_code == MQ_CHECK_CODE) {
      return TRUE;
    }
  }
  return FALSE;
}

#endif  // FRAMEWORK_UNIFIED_CLIENT_NS_MESSAGEQUEUE_INCLUDE_NS_MQ_INTERNAL_H_
