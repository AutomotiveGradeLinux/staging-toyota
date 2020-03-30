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

#ifndef COMMUNICATION_SERVER_INCLUDE_CAN_TIMERCTRL_CAN_TIMERCTRL_H_
#define COMMUNICATION_SERVER_INCLUDE_CAN_TIMERCTRL_CAN_TIMERCTRL_H_
/******************************************************************************
 * FILE  :CAN_TimerCtrl.h
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :
 ******************************************************************************/

#include "CAN_Thread.h"

/************************************************************************
*   Macro Definitions             *
************************************************************************/
#define CAN_TIM_RETRY_NUM 3 /* Number of retries for alarm API errors */
/************************************************************************
*   Struct Definitions             *
************************************************************************/

/***************************************************
* TAG       : CAN_TIMER_CTRL_DATA
* ABSTRACT  : Timer management data section structure
****************************************************/
typedef struct {
  uint16_t us_set_tim; /* Timeout value */
  uint16_t us_tim_cnt; /* Measured value of timer */
} CAN_TIMER_CTRL_DATA;

/***************************************************
* TAG       : CAN_FREQTRANS_TIMER
* ABSTRACT  : Periodic transmission timer management table structure
****************************************************/
typedef struct {
  uint16_t us_num;                                     /* Number of timer registrations  */
  uint8_t reserve[2];                                 /* Reserve   */
  CAN_TIMER_CTRL_DATA data[CAN_FREQTRANS_TIMER_DATA]; /* Timer management data */
} CAN_FREQTRANS_TIMER;

/***************************************************
* TAG       : CAN_COMMWATCH_TIMER
* ABSTRACT  : Disruption monitoring timer management table structure
****************************************************/
typedef struct {
  uint16_t us_num;                                     /* Number of timer registrations  */
  uint8_t reserve[2];                                 /* Reserve   */
  CAN_TIMER_CTRL_DATA data[CAN_COMMWATCH_TIMER_DATA]; /* Timer management data */
} CAN_COMMWATCH_TIMER;

/************************************************************************
*   Function Prototype             *
************************************************************************/
RET_CAN CANFreqTransTimerStart(uint8_t, uint16_t, CAN_PROTOCOL_TYPE); /* Periodic transmission timer start processing */
RET_CAN CANFreqTransTimerStop(uint8_t, CAN_PROTOCOL_TYPE);            /* Periodic transmission timer start processing */
RET_CAN CANCommWatchTimerStart(uint8_t, uint16_t, CAN_PROTOCOL_TYPE); /* Communication interruption monitoring timer start processing */
RET_CAN CANCommWatchTimerStop(uint8_t, CAN_PROTOCOL_TYPE);            /* Communication interruption monitoring timer stop processing */
RET_CAN CANCommWatchTimerRenewal(uint8_t, CAN_PROTOCOL_TYPE);         /* Communication interruption monitoring timer update processing */
void CANFreqTransTimeOut(HANDLE h_app, uint16_t, CAN_PROTOCOL_TYPE);  /* CAN periodic transmission timeout confirmation processing */
void CANCommWatchTimeOut(HANDLE h_app, CAN_PROTOCOL_TYPE);            /* CAN Communication Disruption Monitoring Timeout Confirmation Process */
void CANTimerStart(uint16_t, CAN_TIMER_CTRL_DATA*);                   /* CAN Timer Master Processing */
void CANTimerTblInit(void);                                           /* CAN timer table initialization processing */

void CANFreqTimerEntry(CANID canid, uint32_t);
void CANFreqTransTimeOutMap(HANDLE h_app);
RET_CAN CANFreqTransTimerStopMap(CANID);
void CANFreqTimerResetMap(CANID);

#endif  // COMMUNICATION_SERVER_INCLUDE_CAN_TIMERCTRL_CAN_TIMERCTRL_H_
