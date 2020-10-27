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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCH_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCH_H_
#include <native_service/frameworkunified_types.h>
#include <peripheral_service/Canif_API.h>
#include <vector>
#include <map>
#include <string>
#include "CommWatchCommon.h"

typedef struct {
  char notify_name[MAX_NAME_SIZE_APP];
  uint32_t data_id;
  uint8_t comm_watch_flag;
  uint16_t set_time;
  uint16_t timer_cnt;
} CAN_COMM_WATCH_VAL;

typedef std::multimap<CANID, CAN_COMM_WATCH_VAL> CAN_CommWatchTable;
typedef std::pair<const CANID, CAN_COMM_WATCH_VAL> CAN_CommWatchTablePair;
typedef CAN_CommWatchTable::iterator CAN_CommWatchTableIt;

void CANCommWatchInit(void);
EFrameworkunifiedStatus CANCommWatch(HANDLE h_app);
EFrameworkunifiedStatus CANCommWatchTimeout(HANDLE h_app);
void CANCommWatchClear(HANDLE h_app, CANID id);
#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_COMMWATCH_CAN_COMMWATCH_H_
