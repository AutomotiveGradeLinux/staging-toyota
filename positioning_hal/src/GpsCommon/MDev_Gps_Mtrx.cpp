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
/**
* @file MDev_Gps_Mtrx.cpp
*/

/*---------------------------------------------------------------------------*/
// Include files
#include "MDev_Gps_Mtrx.h"
#include "positioning_def.h"

#include  "MDev_Gps_Main.h"
#include  "MDev_Gps_API.h"
#include  "MDev_GpsRecv.h"
#include  "MDev_Gps_Common.h"
#include  "MDev_Gps_Nmea.h"
#include  "MDev_Gps_TimerCtrl.h"

extern uint8_t g_gps_reset_cmd_sending;
/*---------------------------------------------------------------------------*/
// Functions

/********************************************************************************
 * MODULE      : DEV_Gps_Nop
 * ABSTRACT    : No processing
 * FUNCTION    : NOP processing of the matrix 
 * ARGUMENT    : None
 * NOTE        :
 * RETURN      : None
 ********************************************************************************/
void DevGpsNop(void) {
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_InitStart_SendReq
 * ABSTRACT    : Startup confirmation - Transmit request reception matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsInitStartSendReq(void) {
  /** Send Request Receive Common Processing Call */
  SendReqGpsCommon();
  /* -- #GPF_60_040 */
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_InitStart_GPSResetReq
 * ABSTRACT    : Startup confirmation - GPS reset request reception matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsInitStartGPSResetReq(void) {
  /** Reset request reception common processing call */
  GPSResetReqGpsCommon();
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_InitStart_RcvCyclCmd
 * ABSTRACT    : Startup confirmation - Cyclic receive command receive matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsInitStartRcvCyclCmd(void) {
  BOOL* pb_rcverr = &(g_gps_mngr.rcv_err_flag); /* GSP receiver error detection status */

  /* Stop start confirmation monitoring timer */
  (void)DevGpsTimeStop(GPS_STARTUP_TIMER);    /* Ignore -> MISRA-C:2004 Rule 16.10 */ /* QAC 3200 */

  /* GPS receiver anomaly detection timer stopped */
  (void)DevGpsTimeStop(GPS_RECEIVERERR_TIMER);

  /* State transition processing(in operation)    */
  ChangeStatusGpsCommon(GPS_STS_NORMAL);

  // Send NAV-TIMEUTC sentence addition requests
  DevGpsNavTimeUTCAddReq();

  DevGpsWknRolloverGetReq();

  /* Receive command analysis(u-blox) */
  if (g_gps_reset_cmd_sending == FALSE) {
      DevGpsRcvCyclCmd();
  }

  /* If a GPS receiver error is detected, the diagnosis code (current) is deleted. */
  if (*pb_rcverr == TRUE) {
    *pb_rcverr = FALSE;
    DevSendGpsConnectError(FALSE);
  }
    // }
  /* Reset retry counter */
  RtyResetGpsCommon();

  /* Sending pending commands */
  SendSaveCmdGpsCommon();    /* State transition if pending commands exist( -> Sending) */

  /* Cyclic reception data monitoring timer setting    */
  (void)DevGpsTimeSet(GPS_CYCL_TIMER);    /* Ignore -> MISRA-C:2004 Rule 16.10 */ /* QAC 3200 */

  /* GPS receiver anomaly detection timer setting */
  (void)DevGpsTimeSet(GPS_RECEIVERERR_TIMER);

  /* -- #GPF_60_024 */
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_InitStart_CyclDataTimeOut
 * ABSTRACT    : Startup confirmation - Reception cycle data monitoring timeout matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsInitStartCyclDataTimeOut(void) {
    uint8_t ret = RETRY_OFF;

    POSITIONING_LOG("DEV_Gps_InitStart_CyclDataTimeOut");

    g_wcnct_err++;             /* Count number of connection errors       */

    /* Stop all timers         */
    DevGpsTimeStop(GPS_STARTUP_TIMER);

    DevGpsTimeStop(GPS_CYCL_TIMER);

    DevGpsTimeStop(GPS_RECV_ACK_TIMER);

    /* Hard reset judgment processing    */
    ret = HardResetChkGpsCommon();
    if (RETRY_ON == ret) {
      DevGpsResetReq(PNO_NONE, 0);

      SendSaveCmdGpsCommon();   /* State transition if pending commands exist( -> Sending) */
    } else if (RETRY_OFF == ret) {
        /* Fixed number of hard resets completed */
    } else {
        /* Retrying(with hard reset) */
    }
    /* Clear cyclic receive data up to now */
    DevGpsCycleDataClear();
    /* Start confirmation monitoring timer setting */
    DevGpsTimeSet(GPS_STARTUP_TIMER);
    return;
}

/******************************************************************************
@brief      DEV_Gps_InitStart_NaviDataTimeOut<BR>
            Startup confirmation - Navigation providing data monitoring timeout matrix function
@outline    Navigation providing data monitoring timeout matrix processing during startup confirmation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartNaviDataTimeOut(void) {
}

/******************************************************************************
@brief      DEV_Gps_InitStart_DiagClkGuardTimeOut<BR>
            Startup confirmation - Time guard monitoring timeout matrix functions provided by diagnosis
@outline    Diagnosis provision time guard monitoring timeout matrix process during startup checking
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartDiagClkGuardTimeOut(void) {
}

/********************************************************************************
 * MODULE      : DEV_Gps_InitStart_AccOffStart
 * ABSTRACT    : Startup confirmation - Feature ACC-OFF instruction matrix functions
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsInitStartAccOffStart(void) {
}

/******************************************************************************
@brief      DEV_Gps_InitStart_NaviInfoDeliver<BR>
            Startup confirmation - Navigation information provision matrix processing
@outline    Navigation information provision matrix processing during startup confirmation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartNaviInfoDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_InitStart_NaviSpeedDeliver<BR>
            Startup confirmation - Navigation vehicle speed information provision matrix processing
@outline    Navigation speed information provision matrix processing during startup confirmation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartNaviSpeedDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_InitStart_SettingTime<BR>
            Startup confirmation - GPS time setting matrix processing
@outline    GPS time setting matrix process during startup confirmation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartSettingTime(void) {
  POSITIONING_LOG("DEV_Gps_InitStart_SettingTime\n");
  DevGpsCommSettingTime();
  return;
}

/* start #GPF_60_109 */
/******************************************************************************
@brief      DEV_Gps_InitStart_NmeaDataGuardTimeOut<BR>
            Startup confirmation - NMEA monitoring timeout matrix processing
@outline    NMEA data-providing guard monitoring timeout process during startup checking
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsInitStartNmeaDataGuardTimeOut(void) {
}

/**
 * @brief
 *   Startup confirmation - Backup memory read request matrix processing
 */
void DevGpsInitStartBackupDataLoadReq(void) {
  POS_DATETIME st_date_time;
  memset(&st_date_time, 0x00, sizeof(st_date_time));
  /* Import GPS date and time settings */
  DevGpsReadGpsTime(&st_date_time);
  // Todo For Test don't need gstGpsFixCnt??
  // DEV_Gps_ReadGpsFixCnt();     /* Read GPS fix count */
  if (g_is_gpstime_sts == TRUE) {
    DevGpsSettingTime(&st_date_time);  /* Time setting  */
  }
  return;
}

/**
 * @brief
 *   Startup confirmation - Thread stop request matrix processing
 */
void DevGpsInitStartStopReq(void) {
  /* Thread stop processing */
  StopThreadGpsCommon();

  return;
}

/**
 * @brief
 *   Startup confirmation - GPS week adjustment counter notification process
 */
void DevGpsInitStartGpsweekcorcntNtf(void) {
  DevGpsSetGpsweekcorcnt();
  return;
}

/**
 * @brief
 *   Startup confirmation - GPS recovery timeout process
 */
void DevGpsInitStartRecoveryTimeOut(void) {
  return;
}

/**
 * @brief
 *   Startup confirmation - GPS receiver anomaly detection timeout processing
 */
void DevGpsInitStartGpsReceiverErrTimeOut(void) {
  BOOL* pb_rcverr = &(g_gps_mngr.rcv_err_flag); /* GSP receiver error detection status */

  *pb_rcverr = TRUE;
  DevSendGpsConnectError(TRUE);
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_Normal_SendReq
 * ABSTRACT    : In operation - Transmit request reception matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsNormalSendReq(void) {
  /** Store in a pending buffer #GPF_60_040 */
  SendReqGpsCommon();

  /** Send the command #GPF_60_040 */
  SendSaveCmdGpsCommon();
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_Normal_GPSResetReq
 * ABSTRACT    : In operation - GPS reset request reception matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsNormalGPSResetReq(void) {
  /** Store in a pending buffer #GPF_60_040 */
  GPSResetReqGpsCommon();

  /** Send the command #GPF_60_040 */
  SendSaveCmdGpsCommon();
  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_Normal_RcvCyclCmd_Nmea
 * ABSTRACT    : In operation - Cyclic receive command receive matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsNormalRcvCyclCmd(void) {
  /* Cyclic reception data monitoring timer stopped */
  (void)DevGpsTimeStop(GPS_CYCL_TIMER);
  /* GPS receiver anomaly detection timer stopped */
  (void)DevGpsTimeStop(GPS_RECEIVERERR_TIMER);

  /* Notify Vehicle Sensor */
  if (g_gps_reset_cmd_sending == FALSE) {
    DevGpsRcvCyclCmd();
  }

  /* Reset retry counter */
  RtyResetGpsCommon();

  /* Cyclic reception data monitoring timer setting */
  (void)DevGpsTimeSet(GPS_CYCL_TIMER);  /* Ignore -> MISRA-C:2004 Rule 16.10 */ /* QAC 3200 */

  /* GPS receiver anomaly detection timer setting */
  (void)DevGpsTimeSet(GPS_RECEIVERERR_TIMER);

  return;
}

/********************************************************************************
 * MODULE      : DEV_Gps_Normal_CyclDataTimeOut
 * ABSTRACT    : In operation - Reception cycle data monitoring timeout matrix function
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsNormalCyclDataTimeOut(void) {
  /* Cyclic reception timeout common processing call */
  CyclDataTimeOutGpsCommon();
}

/******************************************************************************
@brief      DEV_Gps_Normal_NaviDataTimeOut<BR>
            In operation - Navigation providing data monitoring timeout matrix function
@outline    Navigation providing data monitoring timeout matrix processing during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalNaviDataTimeOut(void) {
}

/******************************************************************************
@brief      DEV_Gps_Normal_DiagClkGuardTimeOut<BR>
            In operation - Time guard monitoring timeout matrix functions provided by diagnosis
@outline    Diagnosis provision time guard monitoring timeout matrix process during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalDiagClkGuardTimeOut(void) {
}

/********************************************************************************
 * MODULE      : DEV_Gps_Normal_AccOffStart
 * ABSTRACT    : Startup confirmation - Feature ACC-OFF instruction matrix functions
 * FUNCTION    :
 * ARGUMENT    :
 * NOTE        :
 * RETURN      :
 ********************************************************************************/
void DevGpsNormalAccOffStart(void) {
}

/******************************************************************************
@brief      DEV_Gps_Normal_NaviInfoDeliver<BR>
            In operation - Navigation information provision matrix processing
@outline    Navigation information provision matrix processing during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalNaviInfoDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_Normal_NaviSpeedDeliver<BR>
            In operation - Navigation vehicle speed information provision matrix processing
@outline    Navigation vehicle speed information provision matrix processing during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalNaviSpeedDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_Normal_SettingTime<BR>
            In operation - GPS time setting matrix processing
@outline    GPS time setting matrix processing during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalSettingTime(void) {
  POSITIONING_LOG("DEV_Gps_Normal_SettingTime");
  DevGpsCommSettingTime();
  /** Send the command */
  SendSaveCmdGpsCommon();

  return;
}

/* start #GPF_60_109 */
/******************************************************************************
@brief      DEV_Gps_Normal_NmeaDataGuardTimeOut<BR>
            In operation - NMEA monitoring timeout matrix processing
@outline    NMEA data-providing guard monitoring timeout process during operation
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsNormalNmeaDataGuardTimeOut(void) {
}

/**
 * @brief
 *   In operation - Backup memory read request matrix processing
 */
void DevGpsNormalBackupDataLoadReq(void) {
  POS_DATETIME st_date_time;
  memset(&st_date_time, 0x00, sizeof(st_date_time));
  /* Import GPS date and time settings */
  DevGpsReadGpsTime(&st_date_time);
  // Todo For test don't need gstGpsFixCnt??
  // DEV_Gps_ReadGpsFixCnt();     /* Read GPS fix count */
  if (g_is_gpstime_sts == TRUE) {
       DevGpsSettingTime(&st_date_time); /* Time setting  */
  }
  return;
}

/**
 * @brief
 *   In operation - Thread stop request matrix processing
 */
void DevGpsNormalStopReq(void) {
  /* Thread stop processing */
  StopThreadGpsCommon();

  return;
}

/**
 * @brief
 *   In operation - GPS week adjustment counter notification process
 */
void DevGpsNormalGpsweekcorcntNtf(void) {
  DevGpsSetGpsweekcorcnt();
  return;
}

/**
 * @brief
 *   In operation - GPS recovery timeout processing
 */
void DevGpsNormalRecoveryTimeOut(void) {
}

/**
 * @brief
 *   In operation - GPS receiver anomaly detection timeout processing
 */
void DevGpsNormalGpsReceiverErrTimeOut(void) {
  BOOL* pb_rcverr = &(g_gps_mngr.rcv_err_flag); /* GSP receiver error detection status */

  *pb_rcverr = TRUE;
  DevSendGpsConnectError(TRUE);
  return;
}

/* ++ #GPF_60_040 */
/******************************************************************************
@brief      DEV_Gps_Send_SendReq<BR>
            Sending - Transmit request reception matrix function
@outline    Processing for receiving a transmission request during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendSendReq(void) {
  SendReqGpsCommon();
  return;
}

/******************************************************************************
@brief      DEV_Gps_Send_GPSResetReq<BR>
            Sending - GPS reset request reception matrix function
@outline    Processing when receiving a GPS reset request during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendGPSResetReq(void) {
  GPSResetReqGpsCommon();
  return;
}

/******************************************************************************
@brief      DEV_Gps_Send_RcvCyclCmd<BR>
            Sending - Cyclic receive command receive matrix function
@outline    Processing at the reception of cyclic reception command during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendRcvCyclCmd(void) {
    /* Cyclic reception data monitoring timer stopped */
    DevGpsTimeStop(GPS_CYCL_TIMER);

    /* GPS receiver anomaly detection timer stopped */
    DevGpsTimeStop(GPS_RECEIVERERR_TIMER);

    /* Notify Vehicle Sensor */
    if (g_gps_reset_cmd_sending == FALSE) {
        DevGpsRcvCyclCmd();
    }

    /* Reset retry counter */
    RtyResetGpsCommon();

    /* Cyclic reception data monitoring timer setting */
    (void)DevGpsTimeSet(GPS_CYCL_TIMER);

    (void)DevGpsTimeSet(GPS_RECEIVERERR_TIMER);

    return;
}

/******************************************************************************
@brief      DEV_Gps_Send_RcvRspCmd<BR>
            Sending - Response command reception matrix function
@outline    Processing when receiving a response command during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendRcvRspCmd(void) {
    TG_GPS_OUTPUT_FORMAT  rcv_cmd = g_gps_mngr.rcv_cmd;
    BOOL b_snd_flag = TRUE;

    /** Check whether the command matches the command being monitored. */
    if (rcv_cmd == g_gps_mngr.resp_cmd) {
        /** When the command being monitored is a cold start request */
        if (g_gps_mngr.resp_rst_flag == GPS_CMD_RESET) {
            POSITIONING_LOG("GPS_CMD_RESET\n");
            b_snd_flag = DevGpsRcvProcGpsResetResp(reinterpret_cast<TG_GPS_RCV_DATA*>
                                                    (reinterpret_cast<void*>(g_gps_msg_rcvr.msgdat)));
        } else if (g_gps_mngr.resp_rst_flag == GPS_CMD_TIMESET) {
            POSITIONING_LOG("GPS_CMD_TIMESET\n");
            DevGpsRcvProcTimeSetResp(reinterpret_cast<TG_GPS_RCV_DATA*>
                                                    (reinterpret_cast<void*>(g_gps_msg_rcvr.msgdat)));
        } else if (g_gps_mngr.resp_rst_flag == GPS_CMD_SENTENCEADD_NAVTIMEUTC) {
          POSITIONING_LOG("GPS_CMD_SENTENCEADD_NAVTIMEUTC\n");
          DevGpsRcvProcNavSvinfoAddResp(reinterpret_cast<TG_GPS_RCV_DATA*>
                                                    (reinterpret_cast<void*>(g_gps_msg_rcvr.msgdat)));
        } else if ( g_gps_mngr.resp_rst_flag == GPS_CMD_WKNROLLOVER ) {
          POSITIONING_LOG("GPS_CMD_WKNROLLOVER\n");
          DevGpsRcvProcWknRolloverGetResp(reinterpret_cast<TG_GPS_RCV_DATA*>
                                                    (reinterpret_cast<void*>(g_gps_msg_rcvr.msgdat)));
        } else {
          POSITIONING_LOG("g_gps_mngr.resp_rst_flag INVALID!! [resp_rst_flag = %d]", g_gps_mngr.resp_rst_flag);
        }

        if (b_snd_flag == TRUE) {
          /** Stop ACK monitoring timer */
          DevGpsTimeStop(GPS_RECV_ACK_TIMER);
          /* State transition processing(in operation)  */
          ChangeStatusGpsCommon(GPS_STS_NORMAL);
          /** Reset retry counter */
          SendRtyResetGpsCommon();
          /** Delete command from pending buffer */
          DeleteSaveCmdGpsCommon();
          /* Sending pending commands */
          SendSaveCmdGpsCommon();
        }
    } else {
    }

    return;
}

/******************************************************************************
@brief      DEV_Gps_Send_RspDataTimeOut<BR>
            Sending - Response monitoring timeout reception matrix function
@outline    Processing at reception of response monitoring timeout during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendRspDataTimeOut(void) {
    DevGpsTimeStop(GPS_RECV_ACK_TIMER);
    /** Send retry count */
    g_gps_mngr.sndcnt++;

    /** Retry judgment */
    if (g_gps_mngr.sndcnt == SNDNG_MAX) {
        /** At the time of retry */
        POSITIONING_LOG("GPS Hw Reset : sndcnt[%d]", g_gps_mngr.sndcnt);

        /** When the command being monitored is a cold start request */
        if (g_gps_mngr.resp_rst_flag == GPS_CMD_RESET) {
          POSITIONING_LOG("g_gps_mngr.sndcnt == SNDNG_MAX (GPS_CMD_RESET)");
          /** Send Reset ResponseCommunication error */
          DevGpsRstAnsSend(g_gps_mngr.resp_pno, g_gps_mngr.resp_rid, GPS_SENDNG);
          g_gps_reset_cmd_sending = FALSE;

        } else if (g_gps_mngr.resp_rst_flag == GPS_CMD_TIMESET) {
            POSITIONING_LOG("g_gps_mngr.sndcnt == SNDNG_MAX (GPS_CMD_TIMESET)");
            DevGpsTimesetAnsSend(g_gps_mngr.resp_pno, g_gps_mngr.resp_rid, GPS_SENDNG);
        } else if (g_gps_mngr.resp_rst_flag == GPS_CMD_SENTENCEADD_NAVTIMEUTC) {
            POSITIONING_LOG("g_gps_mngr.sndcnt == SNDNG_MAX (GPS_CMD_SENTENCEADD_NAVTIMEUTC)");
            DevGpsNavTimeUTCAddReq(); /* Retransmission of requests to add NAV-SVINFO commands */
        } else if (g_gps_mngr.resp_rst_flag == GPS_CMD_WKNROLLOVER) {  /* GPS rollover reference date acquisition response */
            POSITIONING_LOG("g_gps_mngr.sndcnt == SNDNG_MAX (GPS_CMD_WKNROLLOVER)");
            DevGpsWknRolloverGetReq(); /* GPS rollover standard week number acquisition request retransmission */
        } else {
            POSITIONING_LOG("g_gps_mngr.resp_rst_flag INVALID!! [resp_rst_flag = %d]", g_gps_mngr.resp_rst_flag);
        }

        DeleteSaveCmdGpsCommon();

        ChangeStatusGpsCommon(GPS_STS_NORMAL);

        SendSaveCmdGpsCommon();
        /** Reset retry counter */
        SendRtyResetGpsCommon();
    } else {
        POSITIONING_LOG("GPS Send Retry : sndcnt[%d]", g_gps_mngr.sndcnt);
        /** Retransmission of pending command */
        SendSaveCmdGpsCommon();
    }

    return;
}

/******************************************************************************
@brief      DEV_Gps_Send_CyclDataTimeOut<BR>
            Sending - Reception cycle data monitoring timeout matrix function
@outline    Processing when receiving periodic reception data monitoring timeout during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendCyclDataTimeOut(void) {
  CyclDataTimeOutGpsCommon();
}

/******************************************************************************
@brief      DEV_Gps_Send_NaviDataTimeOut<BR>
            Sending - Navigation providing data monitoring timeout matrix function
@outline    Navigation providing data monitoring timeout matrix processing during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendNaviDataTimeOut(void) {
}

/******************************************************************************
@brief      DEV_Gps_Send_DiagClkGuardTimeOut<BR>
            Sending - Time guard monitoring timeout matrix functions provided by diagnosis
@outline    Diagnosis providing time guard monitoring timeout matrix process during sending
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendDiagClkGuardTimeOut(void) {
}

/******************************************************************************
@brief      DEV_Gps_Send_AccOffStart<BR>
            Sending - Feature ACC-OFF instruction reception matrix functions
@outline    Processing when receiving ACC-OFF instruction in function being transmitted
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendAccOffStart(void) {
}

/******************************************************************************
@brief      DEV_Gps_Send_NaviInfoDeliver<BR>
            Sending - Navigation information provision matrix processing
@outline    Navigation information provision matrix processing during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendNaviInfoDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_Send_NaviSpeedDeliver<BR>
            Sending - Navigation vehicle speed information provision matrix processing
@outline    Navigation vehicle speed information provision matrix processing during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendNaviSpeedDeliver(void) {
}

/******************************************************************************
@brief      DEV_Gps_Send_SettingTime<BR>
            Sending - GPS time setting matrix processing
@outline    GPS time setting matrix processing during transmission
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendSettingTime(void) {
  DevGpsCommSettingTime();
  return;
}

/* start #GPF_60_109 */
/******************************************************************************
@brief      DEV_Gps_Send_NmeaDataGuardTimeOut<BR>
            Sending - NMEA monitoring timeout matrix processing
@outline    Sending NMEA monitoring timeouts
@param[in]  none
@param[out] none
@return     none
@retval     none
*******************************************************************************/
void DevGpsSendNmeaDataGuardTimeOut(void) {
}

/**
 * @brief
 *   Sending - Backup memory read request matrix processing
 */
void DevGpsSendBackupDataLoadReq(void) {
  POS_DATETIME st_date_time;

  memset(&st_date_time, 0x00, sizeof(st_date_time));
  /* Import GPS date and time settings */
  DevGpsReadGpsTime(&st_date_time);
  // Todo For test don't need gstGpsFixCnt??
  // DEV_Gps_ReadGpsFixCnt();     /* Read GPS fix count */
  if (g_is_gpstime_sts == TRUE) {
       DevGpsSettingTime(&st_date_time); /* Time setting  */
  }

  return;
}

/**
 * @brief
 *   Sending - Thread stop request matrix processing
 */
void DevGpsSendStopReq(void) {
  /* Thread stop processing */
  StopThreadGpsCommon();
  return;
}

/**
 * @brief
 *   Sending - GPS week adjustment counter notification process
 */
void DevGpsSendGpsweekcorcntNtf(void) {
  DevGpsSetGpsweekcorcnt();
  return;
}

/**
 * @brief
 *   Sending - GPS recovery timeout processing
 */
void DevGpsSendRecoveryTimeOut(void) {
  return;
}

/**
 * @brief
 *   Sending - GPS receiver anomaly detection timeout processing
 */
void DevGpsSendGpsReceiverErrTimeOut(void) {
  BOOL* pb_rcverr = &(g_gps_mngr.rcv_err_flag); /* GSP receiver error detection status */

  *pb_rcverr = TRUE;
  DevSendGpsConnectError(TRUE);
  return;
}

/*---------------------------------------------------------------------------*/
/*EOF*/
