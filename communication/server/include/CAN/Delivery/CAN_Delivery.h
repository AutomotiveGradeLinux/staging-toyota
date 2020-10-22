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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
#include <native_service/frameworkunified_types.h>
#include "CAN_Thread.h"
#include <vector>
#include <map>
#include <string>
#include <can_hal.h>

typedef std::multimap<CANID, std::string> CAN_DeliveryEntryList;
typedef CAN_DeliveryEntryList::iterator CAN_DeliveryEntryListIt;
typedef std::pair<const CANID, std::string> CAN_DeliveryEntryListPair;

void CANDeliveryInit(void);
bool CANDeliveryInsert(CANID canid, std::string s);
EFrameworkunifiedStatus CANDeliveryRcvProcess(HANDLE h_app, CanMessage *msg);
EFrameworkunifiedStatus CANDeliveryEntry(HANDLE h_app);
EFrameworkunifiedStatus CANClearEntry(HANDLE h_app);
EFrameworkunifiedStatus CANDeliverySndMsg(HANDLE h_app, CANID ul_canid, uint8_t n_ta,
  uint8_t uc_dlc, const uint8_t *puc_data,
  PS_CommunicationProtocol cid, enum CanIfEchoBackFlags flag = CANIF_PURERECV);
EFrameworkunifiedStatus CANCommandDeliveryRcvProcess(HANDLE h_app,
                                                 CanMessage *msg, uint8_t cmd);
#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_DELIVERY_CAN_DELIVERY_H_
