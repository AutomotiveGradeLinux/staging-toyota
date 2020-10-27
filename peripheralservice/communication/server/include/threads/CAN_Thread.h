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

#ifndef COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_
#define COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_
#include <peripheral_service/Canif_API.h>
#include <stdlib.h>

#include "com_error_type.h"
#include "communication_communicationlog.h"

#if 0
#define CAN_TX_COMMAND_SIZE (CAN_TRX_HEADER_SIZE + CAN_TX_OPERAND_SIZE)

#define CAN_RX_CANDATA_SIZE CAN_DATA_SIZE /* Receive CANData(DATA#1 ~ #x)size */
/* Reserve the structure if it is not a multiple of 4. */
#define CAN_TX_CANDATA_SIZE CAN_DATA_SIZE /* Send CANData(DATA#1 ~ #x)size */
/* Reserve the structure if it is not a multiple of 4. */
#define CAN_RX_OPERAND_SIZE (CAN_TRX_CANID_SIZE + CAN_TRX_DLC_SIZE + CAN_RX_CANDATA_SIZE)
#define CAN_TX_OPERAND_SIZE (CAN_TRX_CANID_SIZE + CAN_TRX_DLC_SIZE + CAN_TX_CANDATA_SIZE)  // NOLINT(whitespace/line_length)
/* Send operand size */

#endif

#define CAN_CMDSND_DATA_SIZE 0 /* Command transmission data section size (CAN command control) */

#define CAN_TX_CMD_DELIVERY_SIZE 4     /* Transmitting CAN Command Delivery Data Size Common Block */
#define CAN_TX_CMD_FUELCALC_RST_SIZE 0 /* Transmit CAN Command Delivery Data Size CAN Section Burn Cost Reset Request Receive Data */
#define CAN_TX_CMD_STARTUP_FIN_SIZE 3  /* Transmit CAN Command Delivery Data Size CAN Startup Completion Notification Data */
#define CAN_TX_CMD_MRST_INFO_SIZE 33   /* Transmit CAN Command Delivery Data Size CAN Master Reset Information Notification Receive Data */
#define CAN_TX_CMD_VERSION_SIZE 4      /* Transmit CAN Command Delivery Data Size CAN Version Response Receive Data */

#define CAN_TRX_HEADER_SIZE 7     /* Size of send/receive header section (data length excluding operands) */
#define CAN_TRX_CANID_SIZE 4                                                     /* Transmit/Receive CAN ID Size */
#define CAN_TRX_DLC_SIZE 1                                                       /* Transmit/Receive DLC Data Size */
typedef CANIF_RET_API RET_CAN;
EFrameworkunifiedStatus CANCallbackForTimeOut(HANDLE);
#endif  // COMMUNICATION_SERVER_INCLUDE_THREADS_CAN_THREAD_H_