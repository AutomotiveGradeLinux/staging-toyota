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

#ifndef INC_INPUT_UTIL_H_
#define INC_INPUT_UTIL_H_

#include <native_service/frameworkunified_types.h>

struct InputUtilList {
  struct InputUtilList *next;
  struct InputUtilList *prev;
};

#define INPUT_INIT_LIST_HEAD(ptr) \
    (ptr)->next = (ptr); (ptr)->prev = (ptr);

#define input_list_entry(ptr, type, member) \
  (reinterpret_cast<type *>( \
      reinterpret_cast<char *>(ptr) - (size_t)(&(reinterpret_cast<type *>(0))->member)))

#define input_list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head); pos = pos->next)

#define input_list_for_each_safe(pos, n, head) \
  for (pos = (head)->next, n = pos->next; pos != (head); \
    pos = n, n = pos->next)

void InputUtilListAdd(struct InputUtilList *node_new, struct InputUtilList *node_head);
void InputUtilListDelete(struct InputUtilList *node);
int InputUtilMCSend(HANDLE h_message, PCSTR source, UI_32 cmd, UI_32 length, PCVOID data);
int InputUtilSleep(int usec);

#endif  // INC_INPUT_UTIL_H_
