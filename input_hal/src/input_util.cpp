/*
 * @copyright Copyright (c) 2018-2020 TOYOTA MOTOR CORPORATION.
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
#include "input_util.h"

#include <native_service/ns_message_center_if.h>
#include <stdio.h>

#include "input_hal.h"
#include "input_hal_debug.h"

#define INPUT_UTIL_MESSAGE_SEND_RETRY    3
#define INPUT_UTIL_MESSAGE_SEND_WAIT     10000   /* RetryWait:10ms */
/*
 * InputUtilListAdd
 */
void InputUtilListAdd(struct InputUtilList *node_new, struct InputUtilList *node_head) {
  node_new->prev = node_head;
  node_new->next  = node_head->next;
  node_head->next = node_new;
  node_new->next->prev = node_new;
}

/*
 * InputUtilListDelete
 */
void InputUtilListDelete(struct InputUtilList *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

/*
 * InputUtilMCSend
 */
int InputUtilMCSend(HANDLE h_message, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data) {
  int       i = 0;
  EFrameworkunifiedStatus      e_status;                              /* return value */

  do {
    e_status = McSend(h_message, source, cmd, length, data);
    if (eFrameworkunifiedStatusOK == e_status) {
      break;
    }
    InputUtilSleep(INPUT_UTIL_MESSAGE_SEND_WAIT);
  } while (i++ < INPUT_UTIL_MESSAGE_SEND_RETRY);

  if (e_status != eFrameworkunifiedStatusOK) {
    INPUT_ERROR_LOG("ERR: MessageSend=%d \n", e_status);
  }

  return e_status;
}

/*
 * InputUtilSleep
 */
int InputUtilSleep(int usec) {
  struct timespec  req;

  req.tv_sec = 0;
  req.tv_nsec = usec * 1000;
  nanosleep(&req, NULL);

  return HAL_INPUT_RET_NORMAL;
}
