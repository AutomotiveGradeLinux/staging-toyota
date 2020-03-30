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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMAND_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMAND_H_
/******************************************************************************
 * FILE  :CAN_Command.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
 ******************************************************************************/

#include "CAN_Thread.h"

/************************************************************************
*   Macro definitions             *
************************************************************************/

/************************************************************************
*   Struct definitions             *
************************************************************************/

/************************************************************************
*   Function prototype             *
************************************************************************/
EFrameworkunifiedStatus CANCommandTransmission(HANDLE h_app);
RET_CAN CANCommandTxRslt(HANDLE h_app, CAN_MSG_DATA*);
RET_CAN CANCommandDelivery(HANDLE h_app, CAN_MSG_DATA*);

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_COMMAND_CAN_COMMAND_H_
