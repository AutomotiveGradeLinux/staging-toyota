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

/*******************************************************************************
 * FILE  :CAN_TimerCtrl.cpp
 * SYSTEM  :_CWORD107_
 * SUBSYSTEM :EXL process
 * PROGRAM :CAN thread timer management processing
 * Module configuration :CANFreqTransTimerStart()   Periodic transmission timer start processing
 *     :CANFreqTransTimerStop()   Periodic transmission timer stop processing
 *     :CANCommWatchTimerStart()   Communication interruption monitoring timer start processing
 *     :CANCommWatchTimerStop()   Communication interruption monitoring timer stop processing
 *     :CANCommWatchTimerRenewal()  Communication interruption monitoring timer update processing
 *     :CANFreqTransTimeOut()   CAN periodic transmission timeout confirmation processing
 *     :CANCommWatchTimeOut()   CAN Communication Disruption Monitoring Timeout Confirmation Process
 *     :CANTimerStart()     CAN Timer Master Processing
 *     :CANTimerTblInit()    CAN timer table initialization processing
 ******************************************************************************/
#include "CAN_TimerCtrl.h"

#include <string.h>  // NOLINT (build/include)
#include <native_service/frameworkunified_types.h>
#include <peripheral_service/Canif_API.h>
#include <map>
#include "CAN_Thread.h"
#include "CAN_CommWatch.h"
#include "CAN_Transmission.h"
#include "CAN_TransmissionData.h"
#include "CAN_CommWatchData.h"


using std::map;

/*************************************************/
/*           Global variable                      */
/*************************************************/
CAN_FREQTRANS_TIMER  g_st_can_freq_trans_tim[CAN_PROTOCOL_TYPE_TERMINATE]; /* CAN Periodic Transmission Timer Management Table */
CAN_COMMWATCH_TIMER g_st_can_comm_watch_tim[CAN_PROTOCOL_TYPE_TERMINATE]; /* CAN communication interruption monitoring timer management table */
map<CANID, CAN_TIMER_CTRL_DATA> g_map_can_freq_trans_time;   /* Periodic transmission CAN Command management table */

/*******************************************************************************
 * MODULE    : CANFreqTransTimerStart
 * ABSTRACT  : Periodic transmission timer start processing
 * FUNCTION  : Registering a Periodic Transmission Timer (Periodic Timer) 
 * ARGUMENT  : uc_index    : Indexed
 *           : us_time     : Periodic send timer value
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL  : Normal completion
 *      RET_CAN_ERROR_TIMER : Timer acquisition failed
 ******************************************************************************/
RET_CAN CANFreqTransTimerStart(uint8_t uc_index, uint16_t us_time, CAN_PROTOCOL_TYPE type) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */

  /* Timer not started for specified index */
  if ((uint16_t)0 ==  g_st_can_freq_trans_tim[type].data[uc_index].us_set_tim) {
    /* Updating the number of timer registrations */
     g_st_can_freq_trans_tim[type].us_num++;
  }
  /* Regular transmission interval timer registration */
  CANTimerStart(us_time, & g_st_can_freq_trans_tim[type].data[uc_index]);

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransTimerStop
 * ABSTRACT  : Periodic transmission timer stop processing
 * FUNCTION  : Stops the periodic transmission timer (periodic timer). 
 * ARGUMENT  : uc_index    : Indexed
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL  : Normal completion
 *      RET_CAN_ERROR_TIMER : Failed to stop timer
 ******************************************************************************/
RET_CAN CANFreqTransTimerStop(uint8_t uc_index, CAN_PROTOCOL_TYPE type) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */

  if ((uint16_t)0 <  g_st_can_freq_trans_tim[type].data[uc_index].us_set_tim) {
    /* Stop the CAN periodic transmission timer. */
     g_st_can_freq_trans_tim[type].data[uc_index].us_set_tim = 0;
     g_st_can_freq_trans_tim[type].data[uc_index].us_tim_cnt = 0;
     g_st_can_freq_trans_tim[type].us_num--;
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerStart
 * ABSTRACT  : Communication interruption monitoring timer start processing
 * FUNCTION  : Register the communication interruption monitoring timer (single occurrence timer) 
 * ARGUMENT  : uc_index    : Indexed
 *           : usWatchTim  : Communication interruption monitoring time
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL  : Normal completion
 *      RET_CAN_ERROR_TIMER : Timer acquisition failed
 ******************************************************************************/
RET_CAN CANCommWatchTimerStart(uint8_t uc_index, uint16_t us_watch_time, CAN_PROTOCOL_TYPE type) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */
  uint16_t us_val;
  uint16_t us_tim;

  /* Update Timer Sequence Number */
  CANCommWatchTimerSeqNoRenwal(uc_index, &us_val, type);
  us_tim = (uint16_t)((uint32_t)us_watch_time + 1);
  /* Registration of communication interruption monitoring timer */
  CANTimerStart(us_tim, &g_st_can_comm_watch_tim[type].data[uc_index]);
  /* Updating the number of timer registrations */
  g_st_can_comm_watch_tim[type].us_num++;

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerStop
 * ABSTRACT  : Communication interruption monitoring timer stop processing
 * FUNCTION  : Stop the communication interruption monitoring timer (single occurrence timer) 
 * ARGUMENT  : uc_index    : Indexed
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL  : Normal completion
 *      RET_CAN_ERROR_TIMER : Failed to stop timer
 ******************************************************************************/
RET_CAN CANCommWatchTimerStop(uint8_t uc_index, CAN_PROTOCOL_TYPE type) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */

  if ((uint16_t)0 < g_st_can_comm_watch_tim[type].data[uc_index].us_set_tim) {
    /* Timer stop */
    g_st_can_comm_watch_tim[type].data[uc_index].us_set_tim = 0;
    g_st_can_comm_watch_tim[type].data[uc_index].us_tim_cnt = 0;
    g_st_can_comm_watch_tim[type].us_num--;
  }

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimerRenewal
 * ABSTRACT  : Communication interruption monitoring timer update processing
 * FUNCTION  : Update communication interruption monitoring timer 
 * ARGUMENT  : uc_index    : Indexed
 * NOTE      : 
 * RETURN    : RET_CAN_NORMAL  : Normal completion
 *      RET_CAN_ERROR_TIMER : Timer acquisition or stop failed
 ******************************************************************************/
RET_CAN CANCommWatchTimerRenewal(uint8_t uc_index, CAN_PROTOCOL_TYPE type) {
  RET_CAN ret = RET_CAN_NORMAL; /* Return value of this function */
  uint16_t us_time;               /* Communication interruption time */

  /* Stop timer during startup */
  ret = CANCommWatchTimerStop(uc_index, type);
  /* Update Timer Sequence Number */
  (void)CANCommWatchTimerSeqNoRenwal(uc_index, &us_time, type);
  /* Registration of communication interruption monitoring timer */
  CANTimerStart(us_time, &g_st_can_comm_watch_tim[type].data[uc_index]);
  /* Updating the number of timer registrations */
  g_st_can_comm_watch_tim[type].us_num++;

  return (ret);
}

/*******************************************************************************
 * MODULE    : CANFreqTransTimeOut
 * ABSTRACT  : CAN periodic transmission timeout confirmation processing
 * FUNCTION  : CAN periodic transmission timeout confirmation processing
 * ARGUMENT  : *pst_rcv_msg   : Received Message Reference Pointer
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANFreqTransTimeOut(HANDLE h_app, uint16_t timer_seq, CAN_PROTOCOL_TYPE type) {
  uint16_t us_tim_pos;
  uint16_t us_tim_num;

  us_tim_num =  g_st_can_freq_trans_tim[type].us_num;
  /* Timer start  */
  if ((uint16_t)0 < us_tim_num) {
    /* Check the timeout of the CAN periodic transmission timer table */
    for (us_tim_pos = 0; us_tim_pos < (uint16_t)CAN_FREQTRANS_TIMER_DATA; us_tim_pos++) {
      /* When the timer is starting */
      if ((uint16_t)0 <  g_st_can_freq_trans_tim[type].data[us_tim_pos].us_tim_cnt) {
        /* Decrement timer value */
         g_st_can_freq_trans_tim[type].data[us_tim_pos].us_tim_cnt--;
        /* Detect timeouts */
        if ((uint16_t)0 ==  g_st_can_freq_trans_tim[type].data[us_tim_pos].us_tim_cnt) {
          /* Execute the CAN periodic transmission timeout processing */
          if (type == CAN_PROTOCOL_TYPE_CAN) {
            /* Setting data for timeout messages */
            timer_seq = (uint16_t)((uint32_t)us_tim_pos | FREQ_TRANS_SEQNO_CODE);
            FRAMEWORKUNIFIEDLOG(ZONE_CAN_DEBUG, __func__, "CANFreqTxTimeupMsg Execute(us_tim_pos=%d, TimerSeq=%x)", us_tim_pos,
                                                                                                      timer_seq);
            CANFreqTxTimeupMsg(h_app, timer_seq);
          }
          /* Reset timer value and continue timer operation */
           g_st_can_freq_trans_tim[type].data[us_tim_pos].us_tim_cnt =
           g_st_can_freq_trans_tim[type].data[us_tim_pos].us_set_tim;
        }
        us_tim_num--;
        /* Check completed for the number of timer entries */
        if ((uint16_t)0 == us_tim_num) {
          break;
        }
      }
    }
  }
}

/*******************************************************************************
 * MODULE    : CANCommWatchTimeOut
 * ABSTRACT  : CAN Communication Disruption Monitoring Timeout Confirmation Process
 * FUNCTION  : CAN Communication Disruption Monitoring Timeout Confirmation Process
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANCommWatchTimeOut(HANDLE h_app, CAN_PROTOCOL_TYPE type) {
  uint8_t us_tim_pos;
  uint16_t us_tim_num;
  uint16_t us_timer_seq_no;
  RET_CAN ret;

  us_tim_num = g_st_can_comm_watch_tim[type].us_num;
  /* Timer start  */
  if ((uint16_t)0 < us_tim_num) {
    /* Check timeout of CAN communication interruption monitoring timer table */
    for (us_tim_pos = 0; us_tim_pos < (uint8_t)CAN_COMMWATCH_TIMER_DATA; us_tim_pos++) {
      /* When the timer is starting */
      if ((uint16_t)0 < g_st_can_comm_watch_tim[type].data[us_tim_pos].us_tim_cnt) {
        /* Decrement timer value */
        g_st_can_comm_watch_tim[type].data[us_tim_pos].us_tim_cnt--;
        /* Detect timeouts */
        if ((uint16_t)0 == g_st_can_comm_watch_tim[type].data[us_tim_pos].us_tim_cnt) {
          /* Time-out detected */
          us_timer_seq_no = CANCommWatchTimerSeqNoGet(us_tim_pos, type);

          /* Execute CAN communication interruption monitoring timeout processing */
          switch (type) {
            case CAN_PROTOCOL_TYPE_CAN:
              ret = CANCommWatchTimeoutMsg(h_app, us_timer_seq_no);
              if (RET_CAN_NORMAL != ret) {  // LCOV_EXCL_BR_LINE 8: If us_timer_seq_no is valid, it will not return NON-RET_CAN_NORMAL  // NOLINT (whitespace/line_length)
                AGL_ASSERT_NOT_TESTED(); // LCOV_EXCL_LINE 8: same with above  // NOLINT (whitespace/line_length)
                FRAMEWORKUNIFIEDLOG(ZONE_ERR, __func__, "#CAN_thread# CAN CommWatch TimeoutMsg Error");  // LCOV_EXCL_LINE 8: same with above  // NOLINT (whitespace/line_length)
              }
              break;
            default:
              break;
          }

          /* Deletes the CAN communication interruption monitoring timer table. */
          CANCommWatchTimerStop(us_tim_pos, type);
        }
        us_tim_num--;
        /* Check completed for the number of timer entries */
        if (us_tim_num == (uint16_t)0) {
          break;
        }
      }
    }
  }
}

/*******************************************************************************
 * MODULE    : CANTimerStart
 * ABSTRACT  : CAN Timer Master Processing
 * FUNCTION  : CAN Timer Master Processing
 * ARGUMENT  : u_int16             us_time           Setting timer value
 *           : CAN_TIMER_CTRL_DATA *pst_can_timer_tbl  Timer table pointer to edit
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANTimerStart(uint16_t us_time, CAN_TIMER_CTRL_DATA *pst_can_timer_tbl) {
  /* Update the timeout value */
  pst_can_timer_tbl->us_set_tim = us_time;
  /* Begin detecting timeouts */
  pst_can_timer_tbl->us_tim_cnt = us_time;
}

/*******************************************************************************
 * MODULE    : CANTimerTblInit
 * ABSTRACT  : CAN timer table initialization processing
 * FUNCTION  : CAN Timer Table Initialization Processing
 * ARGUMENT  : None
 * NOTE      : 
 * RETURN    : None
 ******************************************************************************/
void CANTimerTblInit(void) {
  for (int i = 0; i < CAN_PROTOCOL_TYPE_TERMINATE; i++) {
    /* Initialization of the CAN periodic transmission timer table */
    memset(& g_st_can_freq_trans_tim[i], 0, sizeof( g_st_can_freq_trans_tim[i]));
    /* Initialization of CAN communication interruption monitoring timer table */
    memset(&g_st_can_comm_watch_tim[i], 0, sizeof(g_st_can_comm_watch_tim[i]));
  }
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Register Transmission Cycle
 *
 * @~english
 * @return  Return value
 * @retval  None
 *
 * @~english
 * @param[in]     <id>      CAN ID
 * @param[in]     <freq>    Transmission cycle
 */
void CANFreqTimerEntry(CANID id, uint32_t freq) {
  CAN_TIMER_CTRL_DATA tmp_timer_data;

  tmp_timer_data.us_set_tim = (uint16_t)freq;
  tmp_timer_data.us_tim_cnt = (uint16_t)freq;

  g_map_can_freq_trans_time[id] = tmp_timer_data;

  return;
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Periodic trnasmission time out
 *
 * @~english
 * @return  Return value
 * @retval  None
 *
 * @~english
 * @param[in]  None
 */
void CANFreqTransTimeOutMap(HANDLE h_app) {
  map<CANID, CAN_TIMER_CTRL_DATA>::iterator it;
  CAN_TIMER_CTRL_DATA timer_data;
  CANID id;

  for (it = g_map_can_freq_trans_time.begin(); it != g_map_can_freq_trans_time.end(); ++it) {
    id = it->first;
    timer_data = it->second;

    if (0 >= timer_data.us_tim_cnt)
      continue;

    timer_data.us_tim_cnt--;

    if ((uint16_t)0 == timer_data.us_tim_cnt) {
      CANTransStartTxMsg(h_app, id);
      timer_data.us_tim_cnt = timer_data.us_set_tim;
    }

    g_map_can_freq_trans_time[id] = timer_data;
  }
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Clear Transmission Cycle
 *
 * @~english
 * @return  Return value
 * @retval  None
 *
 * @~english
 * @param[in]     <id>      CAN ID
 */
RET_CAN CANFreqTransTimerStopMap(CANID id) {
  RET_CAN ret = RET_CAN_NORMAL;
  CAN_TIMER_CTRL_DATA timer_data;

  timer_data.us_set_tim = 0;
  timer_data.us_tim_cnt = 0;

  g_map_can_freq_trans_time[id] = timer_data;

  return (ret);
}

/*!-----------------------------------------------------------------------------
 * @~english
 * @brief    Reset Transmission Cycle
 *
 * @~english
 * @return  Return value
 * @retval  None
 *
 * @~english
 * @param[in]     <id>      CAN ID
 */
void CANFreqTimerResetMap(CANID id) {
  map<CANID, CAN_TIMER_CTRL_DATA>::iterator it;

  it = g_map_can_freq_trans_time.find(id);
  if (it != g_map_can_freq_trans_time.end()) {
    it->second.us_tim_cnt = it->second.us_set_tim;
  }

  return;
}  // LCOV_EXCL_BR_LINE 10: end of line unknown branch
